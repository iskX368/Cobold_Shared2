// Lib_ScaleInformation.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"
#include "Lib_ScaleInformation.h"


#include <vector>




PointInformation::~PointInformation() {}

class PointInformation_impl:public PointInformation
{
public:
	PointInformation_impl() = delete;
	virtual ~PointInformation_impl() {}


	PointInformation_impl(const size_t NumberOfPoints, Difference dif)
		:m_NumPoints(NumberOfPoints), m_Difference(dif), m_IsClosed0(false), m_IsClosed1(false)
	{
		if (dif == Difference::Forward || dif == Difference::Central0) {
			m_IsClosed0 = true;
			m_IsClosed1 = false;
		}
		else {
			m_IsClosed0 = false;
			m_IsClosed1 = true;
		}
	}

	PointInformation_impl(const size_t NumberOfPoints,const Difference dif,const bool closed0,const bool closed1)
		:
		m_NumPoints(NumberOfPoints),
		m_Difference(dif),
		m_IsClosed0(closed0),
		m_IsClosed1(closed1)
	{}


	//#none
	virtual size_t NumPoints()const noexcept override{
		return m_NumPoints;
	}
	//#none
	virtual bool IsClosed0()const noexcept override{
		return m_IsClosed0;
	}
	//#none
	virtual bool IsClosed1()const noexcept override{
		return m_IsClosed1;
	}
	//#none
	virtual Difference Enum_Difference()const noexcept override{
		return m_Difference;
	}

	//#std::out_of_range
	virtual std::pair<double, double> Point2PointInterval(const size_t Point)const override{
		try {
			if (Point >= NumPoints()) {
				throw std::out_of_range("PointInformation::Point2PointInterval : Point");
			}

			double dp = static_cast<double>(Point);
			double ds = Difference2Shift(m_Difference);
			return std::make_pair<double, double>(dp + ds, dp + ds + 1);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double PointRange0()const {
		try {
			return 0. + Difference2Shift(m_Difference);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double PointRange1()const {
		try {
			return static_cast<double>(m_NumPoints) + Difference2Shift(m_Difference);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}


	//ライブラリに公開しない
	template<bool IsEdge1>
	static bool Difference2EdgeIncluded(const PointInformation::Difference dif) {
		if (IsEdge1)
		{
			//1
			switch (dif)
			{
			case PointInformation::Difference::Forward:
				return false;
			case PointInformation::Difference::Backward:
				return true;
			case PointInformation::Difference::Central0:
				return false;
			case PointInformation::Difference::Central1:
				return true;
			case PointInformation::Difference::Central2:
				return true;
			default:
				throw std::out_of_range("PointInformation::Difference2Shift : Unknon Flag");
				break;
			}
		}
		else
		{
			//0
			switch (dif)
			{
			case PointInformation::Difference::Forward:
				return true;
			case PointInformation::Difference::Backward:
				return false;
			case PointInformation::Difference::Central0:
				return true;
			case PointInformation::Difference::Central1:
				return false;
			case PointInformation::Difference::Central2:
				return true;
			default:
				throw std::out_of_range("PointInformation::Difference2Shift : Unknon Flag");
				break;
			}
		}
	}

private:

	//ライブラリに公開しない
	static double Difference2Shift(const PointInformation::Difference dif) {
		switch (dif)
		{
		case PointInformation::Difference::Forward:
			return 0.;
		case PointInformation::Difference::Backward:
			return - 1.;
		case PointInformation::Difference::Central0:
			return 0. - 0.5;
		case PointInformation::Difference::Central1:
			return 0. - 0.5;
		case PointInformation::Difference::Central2:
			return 0. - 0.5;
		default:
			throw std::out_of_range("PointInformation::Difference2Shift : Unknon Flag");
			break;
		}
	}



	size_t m_NumPoints;
	Difference m_Difference;
	bool m_IsClosed0;
	bool m_IsClosed1;
};


std::shared_ptr<PointInformation> PointInformation::make(const size_t NumberOfPoints, PointInformation::Difference dif) {
	try {
		if (NumberOfPoints < 1) {
			throw std::invalid_argument("PointInformation::make : NumberOfPoint < 1");
		}
		return std::make_shared<PointInformation_impl>(NumberOfPoints, dif, PointInformation_impl::Difference2EdgeIncluded<false>(dif), PointInformation_impl::Difference2EdgeIncluded<true>(dif));
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}

bool operator==(const PointInformation& t1, const PointInformation& t2)noexcept {
	if (t1.NumPoints() != t2.NumPoints()) {
		return false;
	}

	if (t1.Enum_Difference() != t2.Enum_Difference()) {
		return false;
	}

	if (t1.IsClosed0() != t2.IsClosed0()) {
		return false;
	}

	if (t1.IsClosed1() != t2.IsClosed1()) {
		return false;
	}

	return true;
}
bool operator!=(const PointInformation& t1, const PointInformation& t2)noexcept {
	return !(t1 == t2);
}



//std::shared_ptr<PointInformation> make_PointInformation(const size_t NumberOfPoints, PointInformation::Difference dif) {
//	try {
//		return std::make_shared<PointInformation_impl>(NumberOfPoints, dif, PointInformation_impl::Difference2EdgeIncluded<false>(dif), PointInformation_impl::Difference2EdgeIncluded<true>(dif));
//	}
//	catch (...) {
//		throw;
//	}
//}




ScaleInformation::~ScaleInformation() {}


//目盛の情報
class ScaleInformation_impl2 :public ScaleInformation {
public:

	using Difference = PointInformation::Difference;

	explicit ScaleInformation_impl2() = delete;
	explicit ScaleInformation_impl2(const ScaleInformation_impl2&) = delete;

	//コンストラクタ雛形　完成したら例外処理を外部へ追い出す
	explicit ScaleInformation_impl2(const std::shared_ptr<PointInformation>& sppointInformation, const std::function<double(const double)>& f)
		:m_IsMonotonicallyIncreasing(true), m_spPointInformation(nullptr), m_upIntervals(nullptr), m_upRepresentatives(nullptr), m_upRange(nullptr), m_IsClosed0(false), m_IsClosed1(false)
	{
		try {
			//ポインタを確認
			if (!sppointInformation) {
				throw std::invalid_argument("");
			}
			//関数を確認
			if (!f) {
				throw std::invalid_argument("");
			}



			//区間を作成
			std::vector<std::pair<double, double>> vi;
			vi.reserve(sppointInformation->NumPoints());
			//代表値を生成
			std::vector<double> vr;
			vr.reserve(sppointInformation->NumPoints());
			double d0 = 0, d1 = 0, re = 0, ran0 = 0, ran1 = 0;
			bool closed0 = false, closed1 = false;
			bool tobeInc = false;


			//ポイントから区間端を得る関数
			auto Point2IntervalEdge = [](const PointInformation& pointInformation, const size_t Point, const std::function<double(const double)>& f, const bool GetSecond) {
				try {
					auto pntint = pointInformation.Point2PointInterval(Point);
					if (GetSecond) {
						return f(pntint.second);
					}
					else
					{
						return f(pntint.first);
					}
				}
				catch (std::out_of_range&) {
					throw;
				}
			};

			const size_t pmax = sppointInformation->NumPoints();
			for (size_t p = 0; p < pmax; p++) {
				d0 = p == 0 ? Point2IntervalEdge(*sppointInformation, p, f, false) : d1;
				d1 = Point2IntervalEdge(*sppointInformation, p, f, true);
				re = f(static_cast<double>(p));

				//初回
				if (p == 0) {

					ran0 = d0;

					if (d0 < d1) {
						tobeInc = true;
						closed0 = sppointInformation->IsClosed0();
						closed1 = sppointInformation->IsClosed1();
					}
					else if (d0 > d1) {
						tobeInc = false;
						closed0 = sppointInformation->IsClosed0();
						closed1 = sppointInformation->IsClosed1();
					}
					else {
						throw std::exception();
					}
				}
				else if (p == pmax - 1) {
					//最終回

					ran1 = d1;

				}

				//増減で場合分け
				if (tobeInc) {
					if (d0 < d1) {
						vi.push_back(std::pair<double, double>(d0, d1));
						vr.push_back(re);
					}
					else {
						throw std::exception();
					}
				}
				else {
					if (d0 > d1) {
						vi.push_back(std::pair<double, double>(d0, d1));
						vr.push_back(re);
					}
					else {
						throw std::exception();
					}
				}
			}

			
			//ここまで異常なかったらセット
			m_IsMonotonicallyIncreasing = tobeInc;
			m_spPointInformation = sppointInformation;
			m_upIntervals = std::make_unique<std::vector<std::pair<double, double>>>(vi);
			m_upRepresentatives = std::make_unique<std::vector<double>>(vr);
			m_upRange = std::make_unique<std::pair<double, double>>(ran0, ran1);
			m_IsClosed0 = closed0;
			m_IsClosed1 = closed1;

			return;
		}
		catch (...) {
			m_spPointInformation.reset();
			m_upIntervals.reset();
			m_upRange.reset();

			//サイズをゼロにしておく
			m_upIntervals = std::make_unique<std::vector<std::pair<double, double>>>(0);

			return;
		}
	}

	//全て初期化
	explicit ScaleInformation_impl2(const bool IsMonoInc,const std::shared_ptr<PointInformation>& sppointInformation, std::vector<std::pair<double,double>>&& Intervals,std::vector<double>&& Representatives,std::pair<double,double>&& Range,const bool IsClosed0,const bool IsClosed1)
		:
		m_IsMonotonicallyIncreasing(IsMonoInc),
		m_spPointInformation(sppointInformation),
		m_upIntervals(std::make_unique<std::vector<std::pair<double, double>>>( std::forward<std::vector<std::pair<double, double>>>(Intervals) )),
		m_upRepresentatives(std::make_unique<std::vector<double>>(std::forward<std::vector<double>>(Representatives))),
		m_upRange(std::make_unique<std::pair<double, double>>(std::forward<std::pair<double, double>>(Range))),
		m_IsClosed0(IsClosed0),
		m_IsClosed1(IsClosed1)
	{}


	virtual const std::shared_ptr<PointInformation>& PointInfo()const {
		if (m_spPointInformation) {
			return m_spPointInformation;
		}
		else {
			throw std::invalid_argument("ScaleInformation::PointInfo : nullptr due to the falure of the construction");
		}
	}

	virtual bool HasCommonPointInfo(const ScaleInformation& scaleInformation)const noexcept {
		return m_spPointInformation == scaleInformation.PointInfo();
	}

	virtual double Convert2AnotherRepresentative(const ScaleInformation& another_ScaleInformation_Sharing_PntInfo, const double Xvalue_Of_ThisScale)const {
		try {
			if (!HasCommonPointInfo(another_ScaleInformation_Sharing_PntInfo)) {
				throw std::invalid_argument("ScaleInformation::Convert2AnotherRepresentative : The PointInformation should be shared.");
			}

			auto interval_index = X2IntervalIndex(Xvalue_Of_ThisScale);

			return another_ScaleInformation_Sharing_PntInfo.IntervalIndex2XRepresentative(interval_index);
		}
		catch (std::invalid_argument&) {
			throw;
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	//Interval の始端を含むならtrue #none
	virtual bool IsClosed0()const noexcept{
		return m_IsClosed0;
	}

	//Interval の終端を含むならtrue #none
	virtual bool IsClosed1()const noexcept{
		return m_IsClosed1;
	}

	//単調増加ならtrue #none
	virtual bool IsMonotonicallyIncreasing()const noexcept {
		return m_IsMonotonicallyIncreasing;
	}

	//区間の個数 #none
	size_t NumIntervals()const noexcept { 
		if (m_upIntervals) {
			return m_upIntervals->size();
		}
		else {
			return 0;
		}
	}

	//区間を取得 #out_of_range, invalid_argument
	const std::pair<double, double>& Interval(const size_t Interval_Index)const {
		try {
			if (m_upIntervals) {
				return m_upIntervals->at(Interval_Index);
			}
			else {
				throw std::invalid_argument("ScaleInformation::Interval : nullptr due to the invalid argument in the construction");
			}
		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::invalid_argument&) {
			throw;
		}
	}

	virtual double IntervalWidth(const size_t Interval_Index)const {
		try {
			if (m_upIntervals) {
				const auto& itvl = Interval(Interval_Index);
				return itvl.second - itvl.first;
			}
			else {
				throw std::invalid_argument("ScaleInformation::Interval : nullptr due to the invalid argument in the construction");
			}
		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::invalid_argument&) {
			throw;
		}
	}

	//全範囲を取得 #invalid_argument
	const std::pair<double, double>& Range()const {
		if (m_upRange) {
			return *m_upRange;
		}
		else {
			throw std::invalid_argument("ScaleInformation::Range : nullptr due to the invalid argument in the construction");
		}
	}

	//全区間がカバーする範囲に属するかどうか #invalid_argument
	bool IsInRange(const double Xvalue)const {
		try {
			return IsBetweenPair(Range(), Xvalue);
		}
		catch (std::invalid_argument&) {
			throw;
		}
	}

	//指定する区間に属するかどうか #out_of_range, invalid_argument
	bool IsInInterval(const double Xvalue, const size_t Interval_Index)const {
		try {
			return IsBetweenPair(Interval(Interval_Index), Xvalue);
		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::invalid_argument&) {
			throw;
		}
	}

	//属する区間のインデックス #out_of_range
	size_t X2IntervalIndex(const double Xvalue)const {

		auto f = [&](std::pair<double, double>& pr)->bool {
			return IsBetweenPair(pr, Xvalue);
		};

		auto result = std::find_if(m_upIntervals->begin(), m_upIntervals->end(), f);

		if (result == m_upIntervals->end()) {
			throw std::out_of_range("ScaleInformation::X2IntervalIndex : No Interval Found");
		}
		else {
			return std::distance(m_upIntervals->begin(), result);
		}
	}

	//属する区間の代表値 #out_of_range
	double X2XRepresentative(const double Xvalue)const {
		try {
			size_t IntervalIndex = X2IntervalIndex(Xvalue);
			return IntervalIndex2XRepresentative(IntervalIndex);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	//区間インデックスから代表値を取得
	double IntervalIndex2XRepresentative(const size_t Interval_Index)const {
		try {
			if (m_upRepresentatives) {
				return m_upRepresentatives->at(Interval_Index);
			}
			else {
				throw std::invalid_argument("ScaleInformation::IntervalIndex2XRepresentative : nullptr due to the invalid argument in the construction");
			}
		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::invalid_argument&) {
			throw;
		}
	}


private:
	/*size_t Pnt2IntervalIndex(const size_t Point)const noexcept {
		if (m_IsMonotonicallyIncreasing) {
			return Point;
		}
		else {
			return m_spPointInformation->NumPoints() - 1 - Point;
		}
	}*/

	bool IsBetweenPair(const std::pair<double, double>& pr, const double Xvalue)const noexcept{
		bool b0 = false, b1 = false;
		if (IsMonotonicallyIncreasing()) {
			//pr.first < pr.second

			b0 = IsClosed0() ? Xvalue >= pr.first : Xvalue > pr.first;
			b1 = IsClosed1() ? Xvalue <= pr.second : Xvalue < pr.second;
		}
		else {
			//pr.first > pr.second

			b0 = IsClosed0() ? Xvalue <= pr.first : Xvalue < pr.first;
			b1 = IsClosed1() ? Xvalue >= pr.second : Xvalue > pr.second;
		}

		return b0 && b1;
	}


	std::unique_ptr<std::vector<std::pair<double, double>>> m_upIntervals;
	std::unique_ptr<std::vector<double>> m_upRepresentatives;
	std::unique_ptr<std::pair<double, double>> m_upRange;
	bool m_IsMonotonicallyIncreasing;
	bool m_IsClosed0;
	bool m_IsClosed1;
	std::shared_ptr<PointInformation> m_spPointInformation;
};


std::unique_ptr<ScaleInformation> ScaleInformation::make(const std::shared_ptr<PointInformation>& sppointInformation, const std::function<double(const double)>& f) {
	try {
		//ポインタを確認
		if (!sppointInformation) {
			throw std::invalid_argument("make_ScaleInformation : sppointInformation is nullptr.");
		}
		//関数を確認
		if (!f) {
			throw std::invalid_argument("make_ScaleInformation : f is nullptr.");
		}

		//区間を作成
		std::vector<std::pair<double, double>> vi;
		vi.reserve(sppointInformation->NumPoints());
		//代表値を生成
		std::vector<double> vr;
		vr.reserve(sppointInformation->NumPoints());
		double d0 = 0, d1 = 0, re = 0, ran0 = 0, ran1 = 0;
		bool closed0 = false, closed1 = false;
		bool tobeInc = false;


		//ポイントから区間端を得る関数
		auto Point2IntervalEdge = [](const PointInformation& pointInformation, const size_t Point, const std::function<double(const double)>& f, const bool GetSecond) {
			try {
				auto pntint = pointInformation.Point2PointInterval(Point);
				if (GetSecond) {
					return f(pntint.second);
				}
				else
				{
					return f(pntint.first);
				}
			}
			catch (std::out_of_range&) {
				throw;
			}
		};

		const size_t pmax = sppointInformation->NumPoints();
		for (size_t p = 0; p < pmax; p++) {
			d0 = p == 0 ? Point2IntervalEdge(*sppointInformation, p, f, false) : d1;
			d1 = Point2IntervalEdge(*sppointInformation, p, f, true);
			re = f(static_cast<double>(p));

			//初回
			if (p == 0) {

				ran0 = d0;

				if (d0 < d1) {
					tobeInc = true;
					closed0 = sppointInformation->IsClosed0();
					closed1 = sppointInformation->IsClosed1();
				}
				else if (d0 > d1) {
					tobeInc = false;
					closed0 = sppointInformation->IsClosed0();
					closed1 = sppointInformation->IsClosed1();
				}
				else {
					throw std::exception();
				}
			}
			else if (p == pmax - 1) {
				//最終回

				ran1 = d1;

			}


			//centralの場合必要な線形かどうかの判断は面倒なのでしない



			//増減で場合分け
			if (tobeInc) {
				if (d0 < d1) {
					vi.push_back(std::pair<double, double>(d0, d1));
					vr.push_back(re);
				}
				else {
					throw std::exception();
				}
			}
			else {
				if (d0 > d1) {
					vi.push_back(std::pair<double, double>(d0, d1));
					vr.push_back(re);
				}
				else {
					throw std::exception();
				}
			}
		}

		std::pair<double, double> ran(ran0, ran1);

		//ここまでエラーなかったら作成
		return std::make_unique<ScaleInformation_impl2>(tobeInc, sppointInformation, std::move(vi), std::move(vr), std::move(ran), closed0, closed1);
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}


std::unique_ptr<ScaleInformation> ScaleInformation::make(const std::shared_ptr<PointInformation>& sppointInformation, const double Pnt2LinearScale_Ratio, const double Pnt2LinearScale_PntOffset) {
	try {
		if (abs(Pnt2LinearScale_Ratio) < DBL_MIN) {
			throw std::invalid_argument("ScaleInformation::make : Pnt2LinearScale_Ratio should be a non-zero value.");
		}

		auto f = [=](const double dPnt) {return (dPnt - Pnt2LinearScale_PntOffset) * Pnt2LinearScale_Ratio; };
		return make(sppointInformation, f);
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}

bool operator==(const ScaleInformation& t1, const ScaleInformation& t2) {
	return !(t1 != t2);
}
bool operator!=(const ScaleInformation& t1, const ScaleInformation& t2) {

	if (t1.PointInfo() != t2.PointInfo()) {//オブジェクト自体は別でもよい
		return false;
	}

	for (size_t i = 0; i < t1.NumIntervals(); i++) {
		if (t1.Interval(i) != t2.Interval(i)) {
			return false;
		}
	}

	return true;
}





ScaledHistogram1D::~ScaledHistogram1D() = default;

class ScaledHistogram1D_impl1 :public ScaledHistogram1D {
public:

	ScaledHistogram1D_impl1() = delete;
	virtual ~ScaledHistogram1D_impl1() = default;

	ScaledHistogram1D_impl1(std::unique_ptr<ScaleInformation>&& up2ScaleInfo, std::unique_ptr<std::vector<double>>&& up2Histogram, std::unique_ptr<std::vector<double>>&& up2RawHistogram, std::unique_ptr<std::vector<double>>&& up2Width, std::unique_ptr<std::vector<double>>&& up2Hsitogram_Error)
		:
		m_up2ScaleInfo(std::forward<std::unique_ptr<ScaleInformation>>(up2ScaleInfo)),
		m_up2Histogram(std::forward<std::unique_ptr<std::vector<double>>>(up2Histogram)),
		m_up2RawHistogram(std::forward<std::unique_ptr<std::vector<double>>>(up2RawHistogram)),
		m_up2Width(std::forward<std::unique_ptr<std::vector<double>>>(up2Width)),
		m_up2Hsitogram_Error(std::forward<std::unique_ptr<std::vector<double>>>(up2Hsitogram_Error))
	{}


	virtual void AddValue(const double Xvalue, const double Weight) {
		try {
			//負の数は例外？
			m_up2RawHistogram->at(m_up2ScaleInfo->X2IntervalIndex(Xvalue)) += Weight;//そのまま加算
		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::exception&) {
			throw;
		}
	}

	virtual void ClearHistogram()noexcept {
		for (auto& elm : *m_up2RawHistogram) {
			elm = 0;
		}
	}

	

	//virtual const std::vector<double>& Get() {
	//	try {
	//		//RefineHistogram();
	//		return *m_up2Histogram;
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	virtual std::vector<double> Get() const {
		try {
			size_t im = m_up2ScaleInfo->NumIntervals();

			std::vector<double> v;
			v.reserve(im);
			for (size_t i = 0; i < im; i++) {
				v.push_back(GetAt(i));
			}

			return std::move(v);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual std::vector<double> GetE() const {
		try {
			size_t im = m_up2ScaleInfo->NumIntervals();

			std::vector<double> v;
			v.reserve(im);
			for (size_t i = 0; i < im; i++) {
				v.push_back(GetEAt(i));
			}

			return std::move(v);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual std::vector<double> GetE1()const {
		try {
			size_t im = m_up2ScaleInfo->NumIntervals();

			std::vector<double> v;
			v.reserve(im);
			for (size_t i = 0; i < im; i++) {
				v.push_back(GetE1At(i));
			}

			return std::move(v);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual const std::vector<double>& GetWidth()const noexcept {
		return *m_up2Width;
	}


	virtual const std::vector<double>& GetRaw()const noexcept {
		return *m_up2RawHistogram;
	}

	virtual double GetAt(const double Xvalue)const {
		try {
			return GetAt(m_up2ScaleInfo->X2IntervalIndex(Xvalue));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetAt(const size_t Pnt)const {
		try {
			//RefineHistogram();
			return m_up2RawHistogram->at(Pnt) / m_up2Width->at(Pnt);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetEAt(const double Xvalue)const {
		try {
			return GetEAt(m_up2ScaleInfo->X2IntervalIndex(Xvalue));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetEAt(const size_t Pnt)const {
		try {
			//RefineHistogram();
			return sqrt(m_up2RawHistogram->at(Pnt)) / m_up2Width->at(Pnt);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetE1At(const double Xvalue)const {
		try {
			return GetE1At(m_up2ScaleInfo->X2IntervalIndex(Xvalue));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetE1At(const size_t Pnt)const {
		try {
			//RefineHistogram();
			return 1 / sqrt(m_up2RawHistogram->at(Pnt));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual const ScaleInformation& ScaleInfo()const noexcept {
		return *m_up2ScaleInfo;
	}

private:
	std::unique_ptr<std::vector<double>> m_up2Width;//これで割る
	std::unique_ptr<std::vector<double>> m_up2RawHistogram;//これに単に加算する
	std::unique_ptr<std::vector<double>> m_up2Histogram;//割った結果をここに入れて返す	//使ってないので消す
	std::unique_ptr<std::vector<double>> m_up2Hsitogram_Error;//エラーバーも			//使ってないので消す

	std::unique_ptr<ScaleInformation> m_up2ScaleInfo;

};


std::unique_ptr<ScaledHistogram1D> ScaledHistogram1D::make(const std::shared_ptr<PointInformation>& sppointInformation, std::function<double(const double)> f_Pnt2HorizontalScale) {
	try {

		//スケール
		auto up2sci = make_ScaleInformation(sppointInformation, f_Pnt2HorizontalScale);

		const auto numIntervals = up2sci->NumIntervals();

		//ヒストグラム
		auto up2wid = std::make_unique<std::vector<double>>();
		up2wid->reserve(numIntervals);

		auto up2rhst = std::make_unique<std::vector<double>>();
		up2rhst->reserve(numIntervals);

		auto up2hst = std::make_unique<std::vector<double>>();
		up2hst->reserve(numIntervals);

		auto up2hst_err = std::make_unique<std::vector<double>>();
		up2hst_err->reserve(numIntervals);

		for (size_t i = 0; i < numIntervals; i++) {
			up2hst->push_back(0);
			up2rhst->push_back(0);
			up2wid->push_back(up2sci->IntervalWidth(i));
			up2hst_err->push_back(0);
		}

		//ここまでエラーないなら作成
		return std::make_unique<ScaledHistogram1D_impl1>(std::move(up2sci), std::move(up2hst), std::move(up2rhst), std::move(up2wid), std::move(up2hst_err));


	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}

std::unique_ptr<ScaledHistogram1D> ScaledHistogram1D::make(const std::shared_ptr<PointInformation>& sppointInformation, const double Pnt2LinearScale_Ratio, const double Pnt2LinearScale_PntOffset) {
	try {

		//関数
		auto&& fLinear = [=](const double dPnt)->double {return (dPnt - Pnt2LinearScale_PntOffset) * Pnt2LinearScale_Ratio; };
		
		return ScaledHistogram1D::make(sppointInformation, std::move(fLinear));
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}



ScaledHistogram2D::~ScaledHistogram2D() {}

class ScaledHistogram2D_impl :public ScaledHistogram2D {
public:

	ScaledHistogram2D_impl() = delete;
	virtual ~ScaledHistogram2D_impl() = default;

	ScaledHistogram2D_impl(std::unique_ptr<ScaleInformation>&& up2ScaleInfo0, std::unique_ptr<ScaleInformation>&& up2ScaleInfo1, std::unique_ptr<std::vector<std::vector<double>>>&& up2Histogram, std::unique_ptr<std::vector<std::vector<double>>>&& up2RawHistogram, std::unique_ptr<std::vector<std::vector<double>>>&& up2Width, std::unique_ptr<std::vector<std::vector<double>>>&& up2Histogram_Error)
		:
		m_bRefined(false),
		m_up2ScaleInfo0(std::forward<std::unique_ptr<ScaleInformation>>(up2ScaleInfo0)),
		m_up2ScaleInfo1(std::forward<std::unique_ptr<ScaleInformation>>(up2ScaleInfo1)),
		m_up2Histogram(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Histogram)),
		m_up2RawHistogram(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2RawHistogram)),
		m_up2Width(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Width)),
		m_up2Histogram_Error(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Histogram_Error))
	{}


	virtual void AddValue(const double Xvalue, const double Yvalue, const double Weight = 1.0) {
		try {
			//負の数は例外？
			m_up2RawHistogram->at(m_up2ScaleInfo0->X2IntervalIndex(Xvalue)).at(m_up2ScaleInfo1->X2IntervalIndex(Yvalue)) += Weight;//そのまま加算
			m_bRefined = false;
		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::exception&) {
			throw;
		}
	}

	virtual void ClearHistogram() noexcept {
		for (auto& elm1 : *m_up2Histogram) {
			for (auto& elm2 : elm1) {
				elm2 = 0;
			}
		}
	}

	virtual const std::vector<std::vector<double>>& Get() {
		try {
			RefineHistogram();
			return *m_up2Histogram;
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual std::vector<std::vector<double>> Get_()const {
		try {
			size_t im = m_up2ScaleInfo0->NumIntervals();
			size_t jm = m_up2ScaleInfo1->NumIntervals();

			std::vector<std::vector<double>> v;
			v.reserve(im);
			for (size_t i = 0; i < im; i++) {

				std::vector<double> vv;
				vv.reserve(jm);
				for (size_t j = 0; j < jm; j++) {
					vv.push_back(GetAt(i, j));
				}

				v.push_back(std::move(vv));
			}

			return std::move(v);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual std::vector<std::vector<double>> GetE()const {
		try {
			
			size_t im = m_up2ScaleInfo0->NumIntervals();
			size_t jm = m_up2ScaleInfo1->NumIntervals();

			std::vector<std::vector<double>> v;
			v.reserve(im);
			for (size_t i = 0; i < im; i++) {
				
				std::vector<double> vv;
				vv.reserve(jm);
				for (size_t j = 0; j < jm; j++) {
					vv.push_back(GetEAt(i, j));
				}

				v.push_back(std::move(vv));
			}

			return std::move(v);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual std::vector<std::vector<double>> GetE1()const {
		try {

			size_t im = m_up2ScaleInfo0->NumIntervals();
			size_t jm = m_up2ScaleInfo1->NumIntervals();

			std::vector<std::vector<double>> v;
			v.reserve(im);
			for (size_t i = 0; i < im; i++) {

				std::vector<double> vv;
				vv.reserve(jm);
				for (size_t j = 0; j < jm; j++) {
					vv.push_back(GetE1At(i, j));
				}

				v.push_back(std::move(vv));
			}

			return std::move(v);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual const std::vector<std::vector<double>>& GetWidth()const noexcept {
		return *m_up2Width;
	}

	virtual const std::vector<std::vector<double>>& GetRaw()const noexcept {
		return *m_up2RawHistogram;
	}

	virtual double GetAt(const double Xvalue, const double Yvalue)const {
		try {
			return GetAt(m_up2ScaleInfo0->X2IntervalIndex(Xvalue), m_up2ScaleInfo1->X2IntervalIndex(Yvalue));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetAt(const size_t Pnt0, const size_t Pnt1)const {
		try {
			//RefineHistogram();
			return m_up2RawHistogram->at(Pnt0).at(Pnt1) / m_up2Width->at(Pnt0).at(Pnt1);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetEAt(const double Xvalue, const double Yvalue)const {
		try {
			return GetEAt(m_up2ScaleInfo0->X2IntervalIndex(Xvalue), m_up2ScaleInfo1->X2IntervalIndex(Yvalue));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetEAt(const size_t Pnt0, const size_t Pnt1)const {
		try {
			//RefineHistogram();
			return sqrt(m_up2RawHistogram->at(Pnt0).at(Pnt1)) / m_up2Width->at(Pnt0).at(Pnt1);
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetE1At(const double Xvalue, const double Yvalue)const {
		try {
			return GetE1At(m_up2ScaleInfo0->X2IntervalIndex(Xvalue), m_up2ScaleInfo1->X2IntervalIndex(Yvalue));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual double GetE1At(const size_t Pnt0, const size_t Pnt1)const {
		try {
			return 1 / sqrt(m_up2RawHistogram->at(Pnt0).at(Pnt1));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual const ScaleInformation& ScaleInfo0()const noexcept {
		return *m_up2ScaleInfo0;
	}

	virtual const ScaleInformation& ScaleInfo1()const noexcept {
		return *m_up2ScaleInfo1;
	}


private:
	void RefineHistogram() {
		try {
			if (!m_bRefined) {
				for (size_t i = 0; i < m_up2ScaleInfo0->NumIntervals(); i++) {
					for (size_t j = 0; j < m_up2ScaleInfo1->NumIntervals(); j++) {
						m_up2Histogram->at(i).at(j) = m_up2RawHistogram->at(i).at(j) / m_up2Width->at(i).at(j);
					}
				}
				m_bRefined = true;
			}
		}
		catch (std::out_of_range&) {
			m_bRefined = false;
			throw;
		}
	}

	bool m_bRefined;

	std::unique_ptr<std::vector<std::vector<double>>> m_up2Width;//これで割る
	std::unique_ptr<std::vector<std::vector<double>>> m_up2RawHistogram;//これに単に加算する
	std::unique_ptr<std::vector<std::vector<double>>> m_up2Histogram;//割った結果をここに入れて返す		//使ってないので消す
	std::unique_ptr<std::vector<std::vector<double>>> m_up2Histogram_Error;//誤差も						//使ってないので消す


	std::unique_ptr<ScaleInformation> m_up2ScaleInfo0;
	std::unique_ptr<ScaleInformation> m_up2ScaleInfo1;
};




std::unique_ptr<ScaledHistogram2D> ScaledHistogram2D::make(const std::shared_ptr<PointInformation>& sppointInformation0, const std::shared_ptr<PointInformation>& sppointInformation1, std::function<double(const double)> f_Pnt2HorizontalScale0, std::function<double(const double)> f_Pnt2HorizontalScale1) {
	try {

		//スケール
		auto up2sci0 = make_ScaleInformation(sppointInformation0, f_Pnt2HorizontalScale0);
		auto up2sci1 = make_ScaleInformation(sppointInformation1, f_Pnt2HorizontalScale1);

		const auto numIntervals0 = up2sci0->NumIntervals();
		const auto numIntervals1 = up2sci1->NumIntervals();


		//ヒストグラム
		auto up2hst = std::make_unique<std::vector<std::vector<double>>>();
		up2hst->reserve(numIntervals0);
		
		auto up2rhst = std::make_unique<std::vector<std::vector<double>>>();
		up2rhst->reserve(numIntervals0);

		auto up2wdth = std::make_unique<std::vector<std::vector<double>>>();
		up2wdth->reserve(numIntervals0);

		auto up2hst_err = std::make_unique<std::vector<std::vector<double>>>();
		up2hst_err->reserve(numIntervals0);

		for (size_t i = 0; i < numIntervals0; i++) {
			
			auto vhst = std::vector<double>();
			auto vrhst = std::vector<double>();
			auto vwdth = std::vector<double>();
			auto vhst_err = std::vector<double>();

			vhst.reserve(numIntervals1);
			vrhst.reserve(numIntervals1);
			vwdth.reserve(numIntervals1);
			vhst_err.reserve(numIntervals1);


			for (size_t j = 0; j < numIntervals1; j++) {
				vhst.push_back(0);
				vrhst.push_back(0);
				vwdth.push_back(up2sci0->IntervalWidth(i) * up2sci1->IntervalWidth(j));//二軸は直交
				vhst_err.push_back(0);
			}

			
			up2hst->push_back(std::move(vhst));
			up2rhst->push_back(std::move(vrhst));
			up2wdth->push_back(std::move(vwdth));
			up2hst_err->push_back(std::move(vhst_err));

		}

		//ここまでエラーないなら作成
		return std::make_unique<ScaledHistogram2D_impl>(std::move(up2sci0), std::move(up2sci1), std::move(up2hst), std::move(up2rhst), std::move(up2wdth), std::move(up2hst_err));

	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}

