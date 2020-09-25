// Lib_XYPairTableToXYDist.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_XYPairTableToXYDist.h"

#include "Lib_ScaleInformation.h"



#include <algorithm>


XYPairTableToXYDist::~XYPairTableToXYDist() {}
XYPairTableToXYDist::UniformScale::~UniformScale() {}




class XYPairTableToXYDist_impl :public XYPairTableToXYDist {
public:
    XYPairTableToXYDist_impl() = delete;
    virtual ~XYPairTableToXYDist_impl() {}


    class UniformScale_impl :public XYPairTableToXYDist::UniformScale {
    public:
        explicit UniformScale_impl() = delete;
        explicit UniformScale_impl(ScaleInformation* p2ScaleInformation_)
            :m_p2sc(p2ScaleInformation_)
        {}
        virtual ~UniformScale_impl() {}

        virtual explicit operator bool()const noexcept override {
            return m_p2sc != nullptr;
        }

        virtual size_t NumBins()const noexcept override {
            return m_p2sc->NumIntervals();
        }

        virtual double Range_Beg()const noexcept override {
            return m_p2sc->Range().first;
        }

        virtual double Range_Last()const noexcept override {
            return m_p2sc->Range().second;
        }

        virtual double Bin()const noexcept override {
            return m_p2sc->Interval(0).second - m_p2sc->Interval(0).first;
        }

        virtual double Pnt2Scale(const size_t Pnt)const override {
            try {
                return m_p2sc->IntervalIndex2XRepresentative(Pnt);
            }
            catch (std::out_of_range&) {
                throw;
            }
        }

    private:
        ScaleInformation* m_p2sc;
    };
    virtual const XYPairTableToXYDist::UniformScale& X()const {
        try {
            if (!m_up2uniscaleX || !(*m_up2uniscaleX)) {
                throw std::invalid_argument("XYPairTableToXYDist::X : Failure in ScaleInformation X construction");
            }
            else {
                return *m_up2uniscaleX;
            }
        }
        catch (std::invalid_argument&) {
            throw;
        }
    }
    virtual const XYPairTableToXYDist::UniformScale& Y()const {
        try {
            if (!m_up2uniscaleY || !(*m_up2uniscaleY)) {
                throw std::invalid_argument("XYPairTableToXYDist::Y : Failure in ScaleInformation Y construction");
            }
            else {
                return *m_up2uniscaleY;
            }
        }
        catch (std::invalid_argument&) {
            throw;
        }
    }

    
    //全て初期化
    XYPairTableToXYDist_impl(std::unique_ptr<ScaleInformation>&& up2scaleX, std::unique_ptr<ScaleInformation>&& up2scaleY, std::unique_ptr<std::vector<std::vector<type_Elm_XYDist>>>&& up2XYDist, std::unique_ptr<UniformScale>&& up2uniscaleX, std::unique_ptr<UniformScale>&& up2uniscaleY)
        :
        m_up2scaleX(std::forward<std::unique_ptr<ScaleInformation>>(up2scaleX)),
        m_up2scaleY(std::forward<std::unique_ptr<ScaleInformation>>(up2scaleY)),
        m_up2XYDist(std::forward<std::unique_ptr<std::vector<std::vector<type_Elm_XYDist>>>>(up2XYDist)),
        m_up2uniscaleX(std::forward<std::unique_ptr<UniformScale>>(m_up2uniscaleX)),
        m_up2uniscaleY(std::forward<std::unique_ptr<UniformScale>>(m_up2uniscaleY))
    {}



    virtual void ClearCounts() noexcept {
        for (auto& vx : *m_up2XYDist) {
            for (auto& elm : vx) {
                elm = 0;
            }
        }
    }
    virtual const type_XYDist& Get()const {
        try {
            if (!m_up2XYDist) {
                throw std::invalid_argument("XYPairTableToXYDist::Get : Failure in XYDist construction");
            }
            else {
                return *m_up2XYDist;
            }
        }
        catch (std::invalid_argument&) {
            throw;
        }
    }
    virtual type_Elm_XYDist At(const size_t i, const size_t j)const {
        try {
            if (!m_up2XYDist) {
                throw std::invalid_argument("XYPairTableToXYDist::Get : Failure in XYDist construction");
            }
            else {
                return m_up2XYDist->at(i).at(j);
            }
        }
        catch (std::invalid_argument&) {
            throw;
        }
        catch (std::out_of_range&) {

        }
    }

    virtual void Appned_XY1DTable_Events_To_XYDist(const std::vector<double>& XYPairTable);
    virtual void Appned_XY1DTable_Events_To_XYDist(const double* XYPairTable, const size_t NumElements);



    virtual bool Exists()const noexcept {
        return (m_up2XYDist != nullptr) && (m_up2scaleX != nullptr) && (m_up2scaleY != nullptr) && (m_up2uniscaleX != nullptr) && (m_up2uniscaleY != nullptr);
    }
private:
    size_t NumBins(const std::unique_ptr<ScaleInformation>& up2scale)const {
        try {
            if (up2scale == nullptr) {
                throw std::invalid_argument("NumBins : nullptr");
            }
            return up2scale->NumIntervals();
        }
        catch (std::invalid_argument&) {
            throw;
        }
    }
    double Range_Beg(const std::unique_ptr<ScaleInformation>& up2scale)const {
        try {
            if (up2scale == nullptr) {
                throw std::invalid_argument("Offset : nullptr");
            }
            return up2scale->Range().first;
        }
        catch (std::invalid_argument&) {
            throw;
        }
    }
    double Range_Last(const std::unique_ptr<ScaleInformation>& up2scale)const {
        try {
            if (up2scale == nullptr) {
                throw std::invalid_argument("Last : nullptr");
            }
            return up2scale->Range().second;
        }
        catch (std::invalid_argument&) {
            throw;
        }
    }
    double Bin(const std::unique_ptr<ScaleInformation>& up2scale)const {
        try {
            if (up2scale == nullptr) {
                throw std::invalid_argument("Bin : nullptr");
            }
            return up2scale->Interval(0).second - up2scale->Interval(0).first;
        }
        catch (std::invalid_argument&) {
            throw;
        }
    }

    std::unique_ptr<ScaleInformation> m_up2scaleX;
    std::unique_ptr<ScaleInformation> m_up2scaleY;
    std::unique_ptr<type_XYDist> m_up2XYDist;

    std::unique_ptr<UniformScale> m_up2uniscaleX;
    std::unique_ptr<UniformScale> m_up2uniscaleY;

};

void XYPairTableToXYDist_impl::Appned_XY1DTable_Events_To_XYDist(const double* XYPairTable, const size_t NumElements) {
    try {
        std::vector<double> XYPairTable_;
        XYPairTable_.reserve(NumElements);
        for (size_t i = 0; i < NumElements; i++) {
            XYPairTable_.push_back(XYPairTable[i]);
        }
        return Appned_XY1DTable_Events_To_XYDist(XYPairTable_);
    }
    catch (std::invalid_argument&) {
        throw;
    }
    catch (std::out_of_range&) {
        throw;
    }
}
void XYPairTableToXYDist_impl::Appned_XY1DTable_Events_To_XYDist(const std::vector<double>& XYPairTable) {
    try {
        //コンストラクタは成功したか
        if (!Exists()) {
            throw std::invalid_argument("Failure in the class construction due to an invalid input parameter");
        }

        //要素数はゼロより大きな偶数？
        if (XYPairTable.size() < 1) {
            throw std::invalid_argument("XYPairTable size < 1");
        }
        if ((XYPairTable.size() & 1ULL) > 0ULL) {
            throw std::invalid_argument("XYPairTable size should be even.");
        }

        //メンバでの要素数の矛盾
        if (m_up2scaleX->NumIntervals() != m_up2XYDist->size()) {
            throw std::invalid_argument("Check the array size cofigurations in X");
        }
        //size_t sizeof_y_direction;
        for (auto t = m_up2XYDist->begin(), t0 = t; t < m_up2XYDist->end(); t++) {
            if (t->size() != m_up2scaleY->NumIntervals()) {
                throw std::invalid_argument("Check the array size cofigurations in Y");
            }
        }


        //実質pair配列で扱うための関数を定義
        auto e2x = [&](const size_t Event_Index) {
            try {
                return XYPairTable.at(2 * Event_Index);
            }
            catch (std::out_of_range&) {
                throw;
            }
        };
        auto e2y = [&](const size_t Event_Index) {
            try {
                return XYPairTable.at(2 * Event_Index + 1);
            }
            catch (std::out_of_range&) {
                throw;
            }
        };

        //変数を定義
        const size_t NumEvents = XYPairTable.size() / 2;
        const size_t NumInX = m_up2scaleX->NumIntervals();
        const size_t NumInY = m_up2scaleY->NumIntervals();


        //格納先の二次元配列を定義
        type_XYDist XYDist;
        XYDist.reserve(NumInX);
        for (size_t i = 0; i < NumInX; i++) {
            std::vector<type_Elm_XYDist> temp;
            temp.reserve(NumInY);
            for (size_t j = 0; j < NumInY; j++) {
                temp.push_back(0);//ゼロで初期化
            }
            XYDist.push_back(std::move(temp));
        }

        //イベント番号と座標のペア
        std::vector<std::pair<size_t, double>> EXTable, EYTable;

        //ペアの大小関係を定義
        auto compare_EV = [](const std::pair<size_t, double>& lhs, const std::pair<size_t, double>& rhs)->bool {
            return lhs.second < rhs.second;
        };

        //Xについて作成してソート
        EXTable.reserve(NumEvents);
        for (size_t i = 0; i < NumEvents; i++) {
            EXTable.push_back(std::pair<size_t, double>(i, e2x(i)));
        }
        std::sort(EXTable.begin(), EXTable.end(), compare_EV);


        //======================================================================================================
        //引数 : vector<pair>::iterator offset, vector<pair>::iterator end, ScaleInformation, DO-SOMETHING Proc
        //		while Pnt0 < Size
        //		
        //			Find (Pnt0, Interval)
        //			
        //			if Interval Found
        //				
        //				Find Pnt1 : The first event out of the current interval
        //				
        //				if Found				
        //					< DO SOMETIHNG FOR [Pnt0, Pnt1) >
        //					Pnt0 = Pnt1
        //				else
        //					// All events over Pnt0 belongs to the current interval, No need to check
        //					< DO SOMETIHNG FOR [Pnt0, Pnt1) >
        //					break;
        //				end if
        //				
        //			else
        //				// All events out of range
        //				break;
        //			end if
        //
        //		end while
        //======================================================================================================

        //サブプロシージャを定義
        auto Proc1_FindEvent_BelongingTo_An_Interval = [](size_t& Dst_Pnt, size_t& Dst_IntervalIndex_Of_Pnt, const ScaleInformation& Scale_Information, const std::vector<std::pair<size_t, double>>& Event_Value_Vector_Sorted, const size_t Offset_Pnt = 0)->bool {
            try {
                if (Offset_Pnt >= Event_Value_Vector_Sorted.size()) {
                    throw std::out_of_range("Proc1_FindEvent_BelongingTo_An_Interval");
                }

                Dst_Pnt = Event_Value_Vector_Sorted.size();

                double value;
                bool Found = false;
                for (size_t i = Offset_Pnt; i < Event_Value_Vector_Sorted.size(); i++) {

                    value = Event_Value_Vector_Sorted.at(i).second;

                    if (!Found) {
                        if (Scale_Information.IsInRange(value)) {
                            Found = true;
                            Dst_Pnt = i;
                            Dst_IntervalIndex_Of_Pnt = Scale_Information.X2IntervalIndex(value);
                            break;
                        }
                    }
                }

                return Found;

            }
            catch (std::out_of_range&) {
                throw;
            }
        };
        auto Proc1_FindEvent_OutOf_The_CurrentInterval = [](size_t& Dst_Pnt_OutOf_The_CurrentInterval, const size_t Start_Pnt, const size_t IntervalIndex_Of_Start_Pnt, const ScaleInformation& Scale_Information, const std::vector<std::pair<size_t, double>>& Event_Value_Vector_Sorted)->bool {
            try {
                if (Start_Pnt >= Event_Value_Vector_Sorted.size()) {
                    throw std::invalid_argument("Proc1_FindEvent_OutOf_The_CurrentInterval : Start_Pnt");
                }
                if (IntervalIndex_Of_Start_Pnt >= Scale_Information.NumIntervals()) {
                    throw std::invalid_argument("Proc1_FindEvent_OutOf_The_CurrentInterval : IntervalIndex_Of_Start_Pnt");
                }

                double value = Event_Value_Vector_Sorted.at(Start_Pnt).second;

                //始まりのインデックスに付随するイベントが与えられた区間に属しているか確認
                if (!Scale_Information.IsInInterval(value, IntervalIndex_Of_Start_Pnt)) {
                    throw std::out_of_range("Proc1_FindEvent_OutOf_The_CurrentInterval : Start_Pnt out of IntervalIndex_Of_Start_Pnt");
                }

                Dst_Pnt_OutOf_The_CurrentInterval = Start_Pnt + 1;

                bool Found = false;
                for (size_t i = Start_Pnt + 1; i < Event_Value_Vector_Sorted.size(); i++) {
                    value = Event_Value_Vector_Sorted.at(i).second;

                    if (!Found) {
                        if (!Scale_Information.IsInInterval(value, IntervalIndex_Of_Start_Pnt)) {
                            Found = true;
                            Dst_Pnt_OutOf_The_CurrentInterval = i;
                            break;
                        }
                    }
                }

                return Found;
            }
            catch (std::out_of_range&) {
                throw;
            }
            catch (std::invalid_argument&) {
                throw;
            }
        };


        //初めてレンジ内に所属するインデックスを探す
        bool Found = false;
        size_t Pnt0 = 0, Pnt1 = 0, Qnt0 = 0, Qnt1 = 0;
        size_t IntervalIndex_Of_Pnt0 = 0, IntervalIndex_Of_Qnt0 = 0;

        while (Pnt0 < EXTable.size())
        {

            Found = Proc1_FindEvent_BelongingTo_An_Interval(Pnt0, IntervalIndex_Of_Pnt0, *m_up2scaleX, EXTable, Pnt1);


            if (Found) {

                Found = Proc1_FindEvent_OutOf_The_CurrentInterval(Pnt1, Pnt0, IntervalIndex_Of_Pnt0, *m_up2scaleX, EXTable);

                if (!Found) {
                    Pnt1 = EXTable.size();
                }

                //---------------------------------DO_SOMETING
                {

                    //Yについて作成してソート
                    EYTable.clear();
                    EYTable.reserve(Pnt1 - Pnt0);
                    size_t ev;
                    for (size_t i = Pnt0; i < Pnt1; i++) {
                        ev = EXTable.at(i).first;
                        EYTable.push_back(std::pair<size_t, double>(ev, e2y(ev)));
                    }
                    std::sort(EYTable.begin(), EYTable.end(), compare_EV);


                    Qnt0 = 0, Qnt1 = 0;
                    IntervalIndex_Of_Qnt0 = 0;
                    while (Qnt0 < EYTable.size())
                    {

                        Found = Proc1_FindEvent_BelongingTo_An_Interval(Qnt0, IntervalIndex_Of_Qnt0, *m_up2scaleY, EYTable, Qnt1);

                        if (Found) {

                            Found = Proc1_FindEvent_OutOf_The_CurrentInterval(Qnt1, Qnt0, IntervalIndex_Of_Qnt0, *m_up2scaleY, EYTable);

                            if (!Found) {
                                Qnt1 = EYTable.size();
                            }

                            //---------------------------------DO_SOMETING
                            //要素数を記録する
                            XYDist.at(IntervalIndex_Of_Pnt0).at(IntervalIndex_Of_Qnt0) = static_cast<type_Elm_XYDist>(Qnt1 - Qnt0);
                            //--------------------------------- DO_SOMETING


                        }
                        else {
                            break;
                        }


                        if (Qnt1 == EYTable.size()) {
                            break;
                        }


                    }

                }
                //--------------------------------- DO_SOMETING

                if (Pnt1 == EXTable.size()) {
                    break;
                }
                else {
                    Pnt0 = Pnt1;
                }

            }
            else {
                break;
            }

        }


        //二次元分布へ加算
        for (size_t i = 0; i < NumInX; i++) {
            for (size_t j = 0; j < NumInY; j++) {
                m_up2XYDist->at(i).at(j) += XYDist.at(i).at(j);
            }
        }


    }
    catch (std::invalid_argument&) {
        throw;
    }
    catch (std::out_of_range&) {
        throw;
    }
    catch (...) {
        throw;
    }
}




std::unique_ptr<XYPairTableToXYDist> XYPairTableToXYDist::make(const size_t NumBins_X, const double Offset_X, const double Bin_X, const size_t NumBins_Y, const double Offset_Y, const double Bin_Y) {
    try {
        //Binの個数は正の整数
        if (NumBins_X < 1) {
            throw std::invalid_argument("make_XYPairTableToXYDist : NumBins_X should be positive.");
        }
        if (NumBins_Y < 1) {
            throw std::invalid_argument("make_XYPairTableToXYDist : NumBins_Y should be positive.");
        }

        //Binは正数
        if (Bin_X <= 0) {
            throw std::invalid_argument("make_XYPairTableToXYDist : Bin_X should be positive.");
        }
        if (Bin_Y <= 0) {
            throw std::invalid_argument("make_XYPairTableToXYDist : Bin_Y should be positive.");
        }

        //区間インデックスからの変換関数
        auto p2x = [=](const double pnt)->double {
            return Offset_X + pnt * Bin_X;
        };
        auto q2y = [=](const double pnt)->double {
            return Offset_Y + pnt * Bin_Y;
        };

        //オブジェクトを作成
        auto sp2pointX = make_PointInformation(NumBins_X, PointInformation::Difference::Central0);
        auto sp2pointY = make_PointInformation(NumBins_Y, PointInformation::Difference::Central0);

        auto up2scaleX = make_ScaleInformation(sp2pointX, p2x);
        auto up2scaleY = make_ScaleInformation(sp2pointY, q2y);

        auto up2uniscaleX = std::make_unique<XYPairTableToXYDist_impl::UniformScale_impl>(up2scaleX.get());
        auto up2uniscaleY = std::make_unique<XYPairTableToXYDist_impl::UniformScale_impl>(up2scaleY.get());

        std::vector<std::vector<XYPairTableToXYDist::type_Elm_XYDist>> v;
        v.reserve(NumBins_X);
        for (size_t i = 0; i < NumBins_X; i++) {
            std::vector<size_t> vv;
            vv.reserve(NumBins_Y);
            for (size_t j = 0; j < NumBins_Y; j++) {
                vv.push_back(0);
            }
            v.push_back(std::move(vv));
        }

        return std::make_unique<XYPairTableToXYDist_impl>(std::move(up2scaleX), std::move(up2scaleY), std::make_unique<std::vector<std::vector<XYPairTableToXYDist::type_Elm_XYDist>>>(v), std::move(up2uniscaleX), std::move(up2uniscaleY));

    }
    catch (std::invalid_argument&) {
        throw;
    }
    catch (std::out_of_range&) {
        throw;
    }
    catch (...) {
        throw;
    }
}