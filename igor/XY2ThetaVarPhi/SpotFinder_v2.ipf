#pragma rtGlobals=3		// Use modern global access method and strict wave access.

//SpotFinder_v2_HowToUse()の中身を参照


static strconstant folnam="root:About_SpotFinder_v2"
static strconstant winnam="SpotFinder_v2"
static strconstant imgwinnam="SpotFinder_v2_Image"
static strconstant sep=";"

//生成ウェーブ名末尾(円扇, Large, Small)
static strconstant wn_tail_LL="_HolePos0"
static strconstant wn_tail_SL="_HolePos1"
static strconstant wn_tail_SS="_HolePos2"
static strconstant wn_tail_LS="_HolePos3"

//列挙番号(1~4)．色の割り振りが変わる．5は固定．
static constant colorLL=2
static constant colorLS=1
static constant colorSS=4
static constant colorSL=3
static constant colorN=5

menu "Macros"
	submenu "SpotFinder_v2"
		"Make wnidow",SpotFinder_v2_Init()
		"How to use",SpotFinder_v2_HowToUse()
	end
end

function SpotFinder_v2_HowToUse()
	
	print "----- SpotFinder_v2 -----"
	print "<校正データのスポットを自動で発見して，各列についてxy座標を格納した二次元ウェーブを生成する>"
	print "\t1, Macrosからウィンドウを作成する"
	print "\t2, ソースウェーブをポップアップから選択する"
	print "\t3, イメージの中心にカーソルを合わせて中心セットボタンを押す"
	print "\t4, ピークを見つけるボタンを押す"
	print "\t5, リストに表示された点とイメージを見比べて，おかしいなら3つの計算パラメータを増減させて再計算"
	print "\t6, 右上の絵の対応する箇所のチェックがされていることを確認して，リストのハイライトを合わせて色付けボタンを押す"
	print "\t7, いずれにも属さない座標が混入していたらその他にチェックを入れて色付けするか，romoveボタンを押す"
	print "\t8, 検出されていないスポットが存在したら，イメージのカーソルを合わせ追加ボタンを押して色を付ける．座標を追加したらangleでソートボタンを押す"
	print "\t9, 大体色付けしたら色と半径でソートボタン押して並べ替える"
	print "\t10, 数え忘れや余計な点がないならセーブボタン押す．グレーまたは無着色の点は記録されない．プロシージャ冒頭の文字列定数から生成ウェーブの末尾を変更できる"
	print "作成日  2019.10.28."
	print "-----------------------"
end



//ディレクトリ
static function/t objdir()
	return "root:"
end
static function/t homedir()
	return folnam+":"
end

//ポップアップ
Function SpotFinder_v2_PopMenuProc(pa) : PopupMenuControl
	STRUCT WMPopupAction &pa

	switch( pa.eventCode )
	
		case 2: // mouse up
			Variable popNum = pa.popNum
			String popStr = pa.popStr
			
			if(cmpstr(pa.ctrlName,"popup0")==0)//Image
				//Duplicae and draw Image
				string back=getdatafolder(1)
				cd $homedir()
				
				wave w=$(objdir()+popStr)
				if(!waveexists(w))
					print "Error in SpotFinder_v2_PopMenuProc : Source image wave not found."
					return 1
				endif
				
				duplicate/o w,$("source")/wave=src
				dowindow/f $imgwinnam
				if(!V_Flag)
					Display /W=(578.4,56,1089,533.6)/n=$imgwinnam
					AppendImage src
					ModifyImage ''#0 ctab= {*,100,PlanetEarth,0}
					ModifyGraph height={Aspect,1}
					ModifyGraph mirror=2
				endif	
				
				Cursor/N=1/P/I/S=1/C=(65535,65535,65535) G ''#0 40,40	
				
				//save the name
				svar sn=gs_sourceName
				sn=popStr
				
				cd $back
			else
			endif
			
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End


//Gets source wave name list
function/t SpotFinder_v2_GetWaveList()
	string back=GetDataFolder(1)
	cd $objdir()
	string wl=WaveList("*",sep,"DIMS:2,CMPLX:0,TEXT:0")
	cd $back
	return wl
end
static function RefineWaveList()
	string listfunc="SpotFinder_v2_GetWaveList()"
	PopupMenu popup0,value=#listfunc
end

//チェックボックス
Function SpotFinder_v2_CheckProc(cba) : CheckBoxControl
	STRUCT WMCheckboxAction &cba
	
	nvar gv_checked=$(homedir()+"gv_checked")
	
	switch( cba.eventCode )
		case 2: // mouse up
			Variable checked = cba.checked
			
			if(cmpstr(cba.ctrlName,"check0")==0)//1
				if(!checked)
					checkbox check0,value=1
				endif
				checkbox check1,value=0
				checkbox check2,value=0
				checkbox check3,value=0
				checkbox check4,value=0
				gv_checked=colorLL
			elseif(cmpstr(cba.ctrlName,"check1")==0)//2
				checkbox check0,value=0
				if(!checked)
					checkbox check1,value=1
				endif
				checkbox check2,value=0
				checkbox check3,value=0
				checkbox check4,value=0
				gv_checked=colorLS
			elseif(cmpstr(cba.ctrlName,"check2")==0)//3
				checkbox check0,value=0
				checkbox check1,value=0
				if(!checked)
					checkbox check2,value=1
				endif
				checkbox check3,value=0
				checkbox check4,value=0
				gv_checked=colorSS
			elseif(cmpstr(cba.ctrlName,"check3")==0)//4
				checkbox check0,value=0
				checkbox check1,value=0
				checkbox check2,value=0
				if(!checked)
					checkbox check3,value=1
				endif
				checkbox check4,value=0
				gv_checked=colorSL
			elseif(cmpstr(cba.ctrlName,"check4")==0)//5
				checkbox check0,value=0
				checkbox check1,value=0
				checkbox check2,value=0
				checkbox check3,value=0
				if(!checked)
					checkbox check4,value=1
				endif
				gv_checked=colorN
			endif
			
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End


Function SpotFinder_v2_MainWindowHook(s)
	STRUCT WMWinHookStruct &s
	
	Variable hookResult = 0
	switch(s.eventCode)
		case 0:				// Activate
			// Handle activate
			//set wave list
			RefineWaveList()
			hookResult=1
			break
		case 2:				//	Kill
			dowindow $imgwinnam
			if(V_Flag)
				killwindow $imgwinnam
			endif
			if(datafolderexists(folnam))
				killdatafolder $folnam
			endif
			hookResult=1
			break
		case 3:			//Mouse down
			wave brct=$(homedir()+"BlackRect_LTRB")
			variable hh=s.mouseLoc.h, vv=s.mouseLoc.v
			if(hh>brct[0] && hh<brct[2] && vv>brct[1] && vv<brct[3])
				//Call the image window
				getwindow kwTopWin,wsizeOuter
				variable pr=V_right,pt=V_top
				dowindow/f $imgwinnam
				if(!V_Flag)
					print "Error in SpotFinder_v2_MainWindowHook : No image window"
					return 2
				endif
				getwindow $imgwinnam,wsize
				MoveWindow/w=$imgwinnam pr, pt, (V_right-V_left)+pr, (V_bottom-V_top)+pt
				dowindow/f $winnam
			endif
			break
		default:
			hookResult=0
			break
	endswitch
	return hookResult		// 0 if nothing done, else 1
End


//Constructs the main window
function SpotFinder_v2_Init()
	
	string back=getdatafolder(1)
	//if folder exists
	if(datafolderexists(folnam))
		killdatafolder $folnam
	endif
	newdatafolder/o/s $folnam
	
	
	//if window already exists
	dowindow/f $winnam
	if(V_Flag)
		return 1
	endif
	
	
	variable/g gv_pc=0,gv_qc=0
	variable/g gv_checked=colorLL
	string/g gs_sourceName=""
	
	
	//colors
	make/free/o/n=(3) TransParent,Red,Yellow,Green,Blue,Grey
	TransParent={65535,65535,65535}
	Red={65280,32768,32768}
	Yellow={65280,65280,32768}
	Green={32768,65280,49152}
	Blue={32768,54528,65280}
	Grey={60928,60928,60928}
	concatenate/o/np=1 {TransParent,Red,Yellow,Green,Blue,Grey},MyColorWaves
	MatrixTranspose MyColorWaves
	
	//waves for the list
	make/o/n=(1,4) pixListMax
	make/o/n=(1,1,2) select=0
	setdimlabel 2,1,backColors,select
	make/t/o/n=(1) plm_t
	
	//Black Rect LTRB
	make/o/n=(4) BlackRect_LTRB={434,24,684,274}
	wave brct=BlackRect_LTRB
	
	//
	PauseUpdate; Silent 1		// building window...
	NewPanel /W=(332,113,1047,616)/K=1
	ShowTools/A
	SetDrawLayer UserBack
	SetDrawEnv fillfgc= (0,0,0)
	DrawRect 434,24,684,274
	SetDrawEnv fillfgc= (65280,0,0)
	DrawOval 526,40,574,101
	SetDrawEnv fillfgc= (65280,0,0)
	DrawOval 547,189,571,221
	SetDrawEnv fillfgc= (65280,0,0)
	DrawPoly 595,146,1,1,{454,153,505,107,504,188,454,153}
	SetDrawEnv fillfgc= (65280,0,0)
	DrawPoly 511,144,1,1,{370,151,326,133,329,169,370,151}
	PopupMenu popup0,pos={11,10},size={131,20},proc=SpotFinder_v2_PopMenuProc,title="Image"
	PopupMenu popup0,mode=2,popvalue="Select Here",value= #"SpotFinder_v2_GetWaveList()"
	Button button0,pos={8,51},size={241,20},proc=SpotFinder_v2_ButtonProc,title="Set the current position as a center"
	ValDisplay valdisp0,pos={260,54},size={61,15},title="pc",fSize=14
	ValDisplay valdisp0,limits={0,0,0},barmisc={0,1000}
	ValDisplay valdisp0,value= #"root:About_SpotFinder_v2:gv_pc"
	ValDisplay valdisp1,pos={332,53},size={62,15},title="qc",fSize=14
	ValDisplay valdisp1,limits={0,0,0},barmisc={0,1000}
	ValDisplay valdisp1,value= #"root:About_SpotFinder_v2:gv_qc"
	SetVariable setvar0,pos={12,99},size={137,18},title="Spot FWHM [pnt]",fSize=14
	SetVariable setvar0,limits={0,inf,1},value= _NUM:2
	SetVariable setvar1,pos={160,98},size={203,18},title="Peak height threshold [%]"
	SetVariable setvar1,fSize=14,limits={0,inf,1},value= _NUM:20
	Button button1,pos={12,162},size={96,20},proc=SpotFinder_v2_ButtonProc,title="Find peaks"
	SetVariable setvar2,pos={25,125},size={124,18},title="Low limit of Z",fSize=14
	SetVariable setvar2,limits={1,inf,0.5},value= _NUM:1
	CheckBox check0,pos={593,76},size={66,14},proc=SpotFinder_v2_CheckProc,title="大円大扇"
	CheckBox check0,fColor=(65280,32768,32768),value= 0
	CheckBox check1,pos={455,97},size={66,14},proc=SpotFinder_v2_CheckProc,title="大円小扇"
	CheckBox check1,labelBack=(0,0,0),fColor=(65280,65280,32768),value= 0,side= 1
	CheckBox check2,pos={458,178},size={66,14},proc=SpotFinder_v2_CheckProc,title="小円小扇"
	CheckBox check2,fColor=(32768,65280,49152),value= 0,side= 1
	CheckBox check3,pos={593,193},size={66,14},proc=SpotFinder_v2_CheckProc,title="小円大扇"
	CheckBox check3,fColor=(32768,54528,65280),value= 1
	CheckBox check4,pos={442,251},size={49,14},proc=SpotFinder_v2_CheckProc,title="その他"
	CheckBox check4,fColor=(60928,60928,60928),value= 0
	ListBox list0,pos={8,198},size={234,297},proc=SpotFinder_v2_ListBoxProc,fSize=14
	ListBox list0,listWave=root:About_SpotFinder_v2:plm_t
	ListBox list0,selWave=root:About_SpotFinder_v2:select
	ListBox list0,colorWave=root:About_SpotFinder_v2:MyColorWaves,row= 31,mode= 3
	Button button2,pos={261,315},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Color"
	Button button3,pos={397,315},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Clear color"
	Button button4,pos={261,360},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Add the point"
	Button button5,pos={397,360},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Remove the point"
	Button button6,pos={261,403},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Sort by polar angle"
	Button button7,pos={397,403},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Sort by color and r"
	Button button8,pos={564,450},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Save XY positions"
	ToolsGrid snap=1,visible=1
	SetWindow kwTopWin,hook(winhook)=SpotFinder_v2_MainWindowHook
	//
	
	
	
	PopupMenu popup0,popvalue="Select a image"
	dowindow/c $winnam
	RefineWaveList()
	
	wave mcw=MyColorWaves
	CheckBox check0,fColor=(mcw[colorLL][0],mcw[colorLL][1],mcw[colorLL][2]),value=1
	CheckBox check1,fColor=(mcw[colorLS][0],mcw[colorLS][1],mcw[colorLS][2]),value=0
	CheckBox check2,fColor=(mcw[colorSS][0],mcw[colorSS][1],mcw[colorSS][2]),value=0
	CheckBox check3,fColor=(mcw[colorSL][0],mcw[colorSL][1],mcw[colorSL][2]),value=0
	CheckBox check4,fColor=(mcw[colorN][0],mcw[colorN][1],mcw[colorN][2]),value=0
	
	
	cd $back
	return 0
end

Window SpotFinder_v2() : Panel
	PauseUpdate; Silent 1		// building window...
	NewPanel /W=(332,113,1047,616)
	ShowTools/A
	SetDrawLayer UserBack
	SetDrawEnv fillfgc= (0,0,0)
	DrawRect 434,24,684,274
	SetDrawEnv fillfgc= (65280,0,0)
	DrawOval 526,40,574,101
	SetDrawEnv fillfgc= (65280,0,0)
	DrawOval 547,189,571,221
	SetDrawEnv fillfgc= (65280,0,0)
	DrawPoly 595,146,1,1,{454,153,505,107,504,188,454,153}
	SetDrawEnv fillfgc= (65280,0,0)
	DrawPoly 511,144,1,1,{370,151,326,133,329,169,370,151}
	PopupMenu popup0,pos={11,10},size={131,20},proc=SpotFinder_v2_PopMenuProc,title="Image"
	PopupMenu popup0,mode=2,popvalue="e17060504_xy",value= #"SpotFinder_v2_GetWaveList()"
	Button button0,pos={8,51},size={241,20},proc=SpotFinder_v2_ButtonProc,title="Set the current position as a center"
	ValDisplay valdisp0,pos={260,54},size={61,15},title="pc",fSize=14
	ValDisplay valdisp0,limits={0,0,0},barmisc={0,1000}
	ValDisplay valdisp0,value= #"root:About_SpotFinder_v2:gv_pc"
	ValDisplay valdisp1,pos={332,53},size={62,15},title="qc",fSize=14
	ValDisplay valdisp1,limits={0,0,0},barmisc={0,1000}
	ValDisplay valdisp1,value= #"root:About_SpotFinder_v2:gv_qc"
	SetVariable setvar0,pos={12,99},size={137,18},title="Spot FWHM [pnt]",fSize=14
	SetVariable setvar0,limits={0,inf,1},value= _NUM:2
	SetVariable setvar1,pos={160,98},size={203,18},title="Peak height threshold [%]"
	SetVariable setvar1,fSize=14,limits={0,inf,1},value= _NUM:20
	Button button1,pos={12,162},size={96,20},proc=SpotFinder_v2_ButtonProc,title="Find peaks"
	SetVariable setvar2,pos={25,125},size={124,18},title="Low limit of Z",fSize=14
	SetVariable setvar2,limits={1,inf,0.5},value= _NUM:1
	CheckBox check0,pos={593,76},size={66,14},proc=SpotFinder_v2_CheckProc,title="大円大扇"
	CheckBox check0,fColor=(65280,32768,32768),value= 0
	CheckBox check1,pos={455,97},size={66,14},proc=SpotFinder_v2_CheckProc,title="大円小扇"
	CheckBox check1,labelBack=(0,0,0),fColor=(65280,65280,32768),value= 0,side= 1
	CheckBox check2,pos={458,178},size={66,14},proc=SpotFinder_v2_CheckProc,title="小円小扇"
	CheckBox check2,fColor=(32768,65280,49152),value= 0,side= 1
	CheckBox check3,pos={593,193},size={66,14},proc=SpotFinder_v2_CheckProc,title="小円大扇"
	CheckBox check3,fColor=(32768,54528,65280),value= 1
	CheckBox check4,pos={442,251},size={49,14},proc=SpotFinder_v2_CheckProc,title="その他"
	CheckBox check4,fColor=(60928,60928,60928),value= 0
	ListBox list0,pos={8,198},size={234,297},proc=SpotFinder_v2_ListBoxProc,fSize=14
	ListBox list0,listWave=root:About_SpotFinder_v2:plm_t
	ListBox list0,selWave=root:About_SpotFinder_v2:select
	ListBox list0,colorWave=root:About_SpotFinder_v2:MyColorWaves,row= 31,mode= 3
	Button button2,pos={261,315},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Color"
	Button button3,pos={397,315},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Clear color"
	Button button4,pos={261,360},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Add the point"
	Button button5,pos={397,360},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Remove the point"
	Button button6,pos={261,403},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Sort by polar angle"
	Button button7,pos={397,403},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Sort by color and r"
	Button button8,pos={564,450},size={120,20},proc=SpotFinder_v2_ButtonProc,title="Save XY positions"
	ToolsGrid snap=1,visible=1
	SetWindow kwTopWin,hook(winhook)=SpotFinder_v2_MainWindowHook
EndMacro


Function SpotFinder_v2_ButtonProc(ba) : ButtonControl
	STRUCT WMButtonAction &ba
	
	variable i
	
	switch( ba.eventCode )
		case 2: // mouse up
			// click code here
			
			if(cmpstr(ba.ctrlName,"button0")==0)//set center
				
				dowindow $imgwinnam
				if(!V_Flag)
					print "Error in button0 : Select the wave and show a Image window"
					return 1
				endif
				variable pc1=pcsr(G,imgwinnam)
				variable qc1=qcsr(G,imgwinnam)
				nvar pc=$(homedir()+"gv_pc")
				nvar qc=$(homedir()+"gv_qc")
				pc=pc1
				qc=qc1
				
			elseif(cmpstr(ba.ctrlName,"button1")==0)//find peaks
				dowindow $imgwinnam
				if(!V_Flag)
					print "Error in button1 : Select the wave and show a Image window"
					return 1
				endif
				GetPixMaxList()
			elseif(cmpstr(ba.ctrlName,"button2")==0)//color
				wave select=$(homedir()+"select")
				nvar gv_checked=$(homedir()+"gv_checked")
				for(i=0;i<dimsize(select,0);i+=1)
					if(select[i][0][0]==1)
						select[i][0][%backColors]=gv_checked
					endif
				endfor
			elseif(cmpstr(ba.ctrlName,"button3")==0)//clear color
				wave select=$(homedir()+"select")
				for(i=0;i<dimsize(select,0);i+=1)
					if(select[i][0][0]==1)
						select[i][0][%backColors]=0
					endif
				endfor
			
			elseif(cmpstr(ba.ctrlName,"button4")==0)//Add
				if(0==SpotFinder_v2_Dialog("The point will be inserted.;Are you sure?",":"))
					wave plm=$(homedir()+"pixListMax")
					wave/t plm_t=$(homedir()+"plm_t")
					wave select=$(homedir()+"select")
					wave srcImg=$(homedir()+"source")
					nvar pc=$(homedir()+"gv_pc")
					nvar qc=$(homedir()+"gv_qc")
					
					variable pp=pcsr(G,imgwinnam),qq=qcsr(G,imgwinnam)
					//同じ点があれば何もしない
					for(i=0;i<dimsize(plm,0);i+=1)
						if(pp==plm[i][0]&&qq==plm[i][1])
							print "The same coordinate was found : row=",i
							return 1
						endif
					endfor
					
					variable InsertedRowIndex=dimsize(plm,0)//選択の最大値
					for(i=0;i<dimsize(plm,0);i+=1)
						if(select[i][0][0]==1)
							InsertedRowIndex=i+1
						endif
					endfor
					//範囲からはみ出していた場合
					variable iri
					if(InsertedRowIndex>dimsize(plm,0))
						iri=dimsize(plm,0)
					elseif(InsertedRowIndex<0)
						iri=0
					else
						iri=InsertedRowIndex
					endif
					insertpoints/m=0 iri,1,plm,plm_t,select
					plm[iri][0]=pp
					plm[iri][1]=qq
					variable/c rthe=r2polar(cmplx( dimdelta(srcImg,0)*(pp-pc), dimdelta(srcImg,1)*(qq-qc) ))
					plm[iri][2]=real(rthe)
					plm[iri][3]=180/pi*imag(rthe)
					
					plm_t[iri]=getPixPosString(srcImg,pp,qq)
					
					select[iri][0][%backColors]=colorN	
				endif				
			elseif(cmpstr(ba.ctrlName,"button5")==0)//Remove
				wave plm=$(homedir()+"pixListMax")
				wave/t plm_t=$(homedir()+"plm_t")
				wave select=$(homedir()+"select")
				
				variable bselected=0,idel=-1
				for(i=0;i<dimsize(select,0);i+=1)
					if(select[i][0][0]>0)
						bselected+=1
						idel=i
						if(bselected>1)
							print "Over 1 elements selected"
							return 1
						endif
					endif
				endfor
				if(bselected==0)
					print "No element selected"
					return 1
				endif
				if(0==SpotFinder_v2_Dialog("The selected point will be deleted.;Countinue?",";"))	
					deletepoints/m=0 idel,1,plm,plm_t,select
				endif
			elseif(cmpstr(ba.ctrlName,"button6")==0)//Sort by phi
				wave plm=$(homedir()+"pixListMax")
				wave/t plm_t=$(homedir()+"plm_t")
				wave select=$(homedir()+"select")
				//sort
				make/free/o/n=(dimsize(plm,0)) plm_phi=plm[p][3],exc=p
				sort,plm_phi,exc
				
				duplicate/free/o plm,plm1
				plm=plm1[exc[p]][q]
				
				duplicate/free/o/t plm_t,plm_t1
				plm_t=plm_t1[exc[p]]
				
				duplicate/free/o select,select1
				select=select1[exc[p]][q][r]
			elseif(cmpstr(ba.ctrlName,"button7")==0)//Sort by color and r
				wave plm=$(homedir()+"pixListMax")
				wave/t plm_t=$(homedir()+"plm_t")
				wave select=$(homedir()+"select")
				
				duplicate/o plm,plm1
				duplicate/o/t plm_t,plm_t1
				duplicate/o select,select1
				
				
				make/o/n=(6) ind_count=0
				make/o/n=(dimsize(plm,0),6) ind=-1
				variable vcolor
				for(i=0;i<dimsize(plm,0);i+=1)
					vcolor=select[i][0][%backColors]
					ind[ind_count[vcolor]][vcolor]=i
					ind_count[vcolor]+=1
				endfor
				
				variable i0=0,nitem
				for(vcolor=0;vcolor<6;vcolor+=1)
					nitem=ind_count[vcolor]
					if(nitem>0)
						make/o/n=(nitem) plm_ind=ind[p][vcolor]
						make/o/n=(nitem) exc=p,plm_r=plm[ plm_ind[p] ][2]//r
						sort plm_r,exc
						plm1[i0,i0+nitem-1][]=plm[ plm_ind[exc[p-i0]] ][q]
						plm_t1[i0,i0+nitem-1]=plm_t[ plm_ind[exc[p-i0]] ]
						select1[i0,i0+nitem-1][][]=select[ plm_ind[exc[p-i0]] ][q][r]
						i0+=nitem
					endif
				endfor
				
				plm=plm1
				plm_t=plm_t1
				select=select1
			
			elseif(cmpstr(ba.ctrlName,"button8")==0)//save
				svar sn=$(homedir()+"gs_sourceName")
				if(strlen(sn)<1)
					print "Select image"
					return 1
				endif
				wave src=$(homedir()+"source")
				
				string wnLL=sn+wn_tail_LL
				string wnLS=sn+wn_tail_LS
				string wnSS=sn+wn_tail_SS
				string wnSL=sn+wn_tail_SL
				string sLL="   "+wnLL+" (Large circle, Large sector);"
				string sLS="   "+wnLS+" (Large circle, Small sector);"
				string sSS="   "+wnSS+" (Small circle, Small sector);"
				string sSL="   "+wnSL+" (Small circle, Large sector);"
				
				if(0==SpotFinder_v2_Dialog("Following  XY pos waves will be created.;;"+sLL+sLS+sSS+sSL+";Continue ?",";"))
					wave plm=$(homedir()+"pixListMax")
					wave select=$(homedir()+"select")
					nvar pc=$(homedir()+"gv_pc"),qc=$(homedir()+"gv_qc")
					
					variable cnt
					variable x0=dimoffset(src,0), y0=dimoffset(src,1)
					variable dx=dimdelta(src,0), dy=dimdelta(src,1)
					//LL
					make/o/n=(dimsize(plm,0),2) $(objdir()+wnLL)/wave=dst
					cnt=0
					dst[cnt][0]=x0+pc*dx
					dst[cnt][1]=y0+qc*dy
					cnt+=1
					i=0
					do
						if(i>=dimsize(plm,0))
							break
						endif
						if(select[i][0][%backColors]==ColorLL)
							dst[cnt][0]=x0+plm[i][0]*dx
							dst[cnt][1]=y0+plm[i][1]*dy
							cnt+=1
						endif
						i+=1
					while(1)
					//LS
					deletepoints/m=0 cnt,dimsize(dst,0)-cnt,dst
					make/o/n=(dimsize(plm,0),2) $(objdir()+wnLS)/wave=dst
					cnt=0
					dst[cnt][0]=x0+pc*dx
					dst[cnt][1]=y0+qc*dy
					cnt+=1
					i=0
					do
						if(i>=dimsize(plm,0))
							break
						endif
						if(select[i][0][%backColors]==ColorLS)
							dst[cnt][0]=x0+plm[i][0]*dx
							dst[cnt][1]=y0+plm[i][1]*dy
							cnt+=1
						endif
						i+=1
					while(1)
					deletepoints/m=0 cnt,dimsize(dst,0)-cnt,dst
					//SS
					make/o/n=(dimsize(plm,0),2) $(objdir()+wnSS)/wave=dst
					cnt=0
					dst[cnt][0]=x0+pc*dx
					dst[cnt][1]=y0+qc*dy
					cnt+=1
					i=0
					do
						if(i>=dimsize(plm,0))
							break
						endif
						if(select[i][0][%backColors]==ColorSS)
							dst[cnt][0]=x0+plm[i][0]*dx
							dst[cnt][1]=y0+plm[i][1]*dy
							cnt+=1
						endif
						i+=1
					while(1)
					deletepoints/m=0 cnt,dimsize(dst,0)-cnt,dst
					//SL
					make/o/n=(dimsize(plm,0),2) $(objdir()+wnSL)/wave=dst
					cnt=0
					dst[cnt][0]=x0+pc*dx
					dst[cnt][1]=y0+qc*dy
					cnt+=1
					i=0
					do
						if(i>=dimsize(plm,0))
							break
						endif
						if(select[i][0][%backColors]==ColorSL)
							dst[cnt][0]=x0+plm[i][0]*dx
							dst[cnt][1]=y0+plm[i][1]*dy
							cnt+=1
						endif
						i+=1
					while(1)
					deletepoints/m=0 cnt,dimsize(dst,0)-cnt,dst
					
				endif
			else
			endif
			
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End



//リストの文字列を返す
static function/t getPixPosString(srcImg,pp,qq)
	wave srcImg
	variable pp,qq//ポイント
	
	string str
	sprintf str,"(%5.2f,%5.2f)[%3d,%3d]",dimoffset(srcImg,0)+dimdelta(srcImg,0)*pp,dimoffset(srcImg,1)+dimdelta(srcImg,1)*qq,pp,qq
	return str
end
//リスト用ウェーブに新しいピクセル情報を追加する
static function insertANewPixel(srcImg,pixListMax,pixListMax_t,Select,pp,qq,pc,qc,InsertedRowIndex)
	wave srcImg
	wave pixListMax
	wave/t pixListMax_t
	wave Select
	variable pp,qq//ポイント
	variable pc,qc
	variable InsertedRowIndex//挿入位置
	
	wave plm=pixListMax
	wave/t plm_t=pixListMax_t
	wave select=Select
	
	//範囲からはみ出していた場合
	variable iri
	if(InsertedRowIndex>dimsize(plm,0))
		iri=dimsize(plm,0)
	elseif(InsertedRowIndex<0)
		iri=0
	else
		iri=InsertedRowIndex
	endif
	
	insertpoints/m=0 iri,1,plm,plm_t,select
	plm[iri][0]=pp
	plm[iri][1]=qq
	variable/c rthe=r2polar(cmplx( dimdelta(srcImg,0)*(pp-pc), dimdelta(srcImg,1)*(qq-qc) ))
	plm[iri][2]=real(rthe)
	plm[iri][3]=180/pi*imag(rthe)
	
	plm_t[iri]=getPixPosString(srcImg,pp,qq)
	
	select[iri][0][%backColors]=colorN
	
end
static function SpotFinder_v2_Dialog(strList,sep)
	string strList,sep
	
	string back=getdatafolder(1)
	string dfn="SpotFinder_v2_Dialog_dfn"
	newdatafolder/o/s $dfn
	//dfref fdf=newfreedatafolder()
	//cd newfreedatafolder()
	variable/g gv_return=-1
	string/g gs_dialog="dialog"
	
	//window initial position
	variable wL0=750,wT0=400
	//window width
	variable ww=400
	
	//string initial position
	variable h0=20,v0=20
	//string vertical shift
	variable dv=15
	//string lines
	variable nLine=ItemsInList(strList,sep)
	
	//button size
	variable bsizew=round(ww*0.35),bsizeh=25
	//button positions
	variable bh_dist=round(ww*0.10)
	variable bv0=h0+nLine*dv+20
	variable bh0=bh_dist
	variable bv1=bv0
	variable bh1=bh_dist+bsizew+bh_dist
	
	//window height
	variable wh=bsizeh+20*2+h0+dv*nLine
	//window RB
	variable wR0=wL0+ww,wB0=wT0+wh
	
	
	PauseUpdate; Silent 1		// building window...
	NewPanel/k=2 /W=(wL0,wT0,wR0,wB0)/n=$gs_dialog
	SetDrawLayer UserBack
	variable i
	for(i=0;i<ItemsInList(strList,sep);i+=1)
		DrawText h0,v0+dv*i,StringFromList(i,strList,sep)
	endfor
	Button button0,pos={bh0,bv0},size={bsizew,bsizeh},proc=SpotFinder_v2_Dialog_ButtProc,title="yes"
	Button button1,pos={bh1,bv1},size={bsizew,bsizeh},proc=SpotFinder_v2_Dialog_ButtProc,title="no"
	
	PauseForUser dialog
	
	variable ans=gv_return
	cd $back
	if(datafolderexists(dfn))
		killdatafolder $dfn
	endif
	return ans
end
function SpotFinder_v2_Dialog_ButtProc(ctrlName) : ButtonControl
	string ctrlName
	nvar gv=gv_return
	svar gs=gs_dialog
	if(cmpstr(ctrlName,"button0")==0)
		gv=0
	elseif(cmpstr(ctrlName,"button1")==0)
		gv=1
	endif
	dowindow $gs
	if(V_Flag)
		killwindow $gs
	endif
end





//極大点を見つける
static function GetPixMaxList()
	string back=getdatafolder(1)
	cd homedir()
	
	//Filter
	wave src=source
	duplicate/o src,filtered
	
	//Filter function
	//matrixfilter/b=0/n=3/p=1 median filtered
	matrixfilter/b=0/n=3/p=2 avg filtered
	//matrixfilter/b=0/n=3/p=1 median filtered
	
	wave src=filtered
	duplicate/o src, peakColored
	wave dst=peakColored
	
	//Spot size
	controlinfo/w=$winnam setvar0
	variable halfWidth=V_Value
	variable spotSizeP=halfWidth*2
	variable spotSizeQ=halfWidth*2
	
	//height threshold
	controlinfo/w=$winnam setvar1
	variable PeeakHeight_Threthold=V_Value
	
	//Low limit of z
	controlinfo/w=$winnam setvar2
	variable zmin=V_Value
	
	//Center
	nvar pc=gv_pc
	nvar qc=gv_qc
	
	
	variable count
	duplicate/free/o src,detected
	detected=0
	
	variable i,j
	
	//Peak height
	for(j=0;j<dimsize(src,1);j+=1)
		for(i=0;i+spotSizeP<dimsize(src,0);i+=1)//q:fixed, scan p
			
			make/free/o/n=(spotSizeP) w=src[p+i][j]
			setscale/p x,dimoffset(src,0)+dimdelta(src,0)*i,dimdelta(src,0),w
			
			//if detected
			if(getLength(w)>PeeakHeight_Threthold)
				detected[i+halfWidth][j]+=1
			endif
			
		endfor
	endfor
	for(i=0;i<dimsize(src,0);i+=1)
		for(j=0;j+spotSizeQ<dimsize(src,1);j+=1)//p:fixed, scan q
			
			
			make/free/o/n=(spotSizeQ) w=src[i][j+p]
			setscale/p y,dimoffset(src,1)+dimdelta(src,1)*j,dimdelta(src,1),w
			
			//if detected
			if(getLength(w)>PeeakHeight_Threthold)
				detected[i][j+halfWidth]+=2
			endif
			
		endfor
	endfor
	
	
	
	//Peak absolute value
	for(i=0;i<dimsize(detected,0);i+=1)
		for(j=0;j<dimsize(detected,1);j+=1)
			if(detected[i][j]==3)
				if(src[i][j]>zmin)
					detected[i][j]+=4
				endif
			endif
		endfor
	endfor
	
	
	
	make/o/n=(dimsize(src,0)*dimsize(src,1),7) pixList//Detected pix list  0:p,1:q, 2~5:neighbors' indice, 6:group index(All the neighbors will have the same value.)
	count=0
	for(i=0;i<dimsize(detected,0);i+=1)
		for(j=0;j<dimsize(detected,1);j+=1)
			if(detected[i][j]==7)
				dst[i][j]=nan
				
				pixList[count][0]=i
				pixList[count][1]=j
				pixList[count][2,5]=-1
				pixList[count][6]=-1
				
				count+=1
			else
				dst[i][j]=src[i][j]
			endif
		endfor
	endfor
	deletepoints/m=0 count,dimsize(pixList,0)-count,pixList
	
	//Find neighboring pixels
	variable pp,qq
	for(i=0;i<dimsize(pixList,0);i+=1)
		pp=pixList[i][0]
		qq=pixList[i][1]
		for(j=0;j<dimsize(pixList,0);j+=1)
			if(pixList[j][0]==pp+1 && pixList[j][1]==qq)
				pixList[i][2]=j
			elseif(pixList[j][0]==pp && pixList[j][1]==qq+1)
				pixList[i][3]=j
			elseif(pixList[j][0]==pp-1 && pixList[j][1]==qq)
				pixList[i][4]=j
			elseif(pixList[j][0]==pp && pixList[j][1]==qq-1)
				pixList[i][5]=j
			endif
		endfor		
	endfor
	
	//Set group index
	variable group=0
	for(i=0;i<dimsize(pixList,0);i+=1)
		if(pixList[i][6]<0)
			wave w=findNei(pixList,dimsize(src,0)*dimsize(src,1),i)
			
			for(j=0;j<dimsize(w,0);j+=1)
				pixList[w[j]][6]=group
			endfor
			group+=1
			
		endif
	endfor
	
	//sort by the group index
	make/free/o/n=(dimsize(pixList,0)) key=pixList[p][6],exc=p
	sort key,exc
	duplicate/free/o pixList,pixList1
	pixList=pixList1[exc[p]][q]
	
	//Find the pixel with the maximum z value in the group
	make/o/n=(group,4) pixListMax//Representitives in each group	0:p, 1:q, 2:r[mm], 3:phi[deg]
	variable g0=0,g1=0,br=0
	do
		g0=g1
		i=g0+1
		do
			if(i>=dimsize(pixList,0))
				g1=i
				br=1
				break
			elseif(pixList[i][6]!=pixList[g0][6])
				g1=i
				break
			else
				i+=1
			endif
		while(1)
		
		make/free/o/n=(g1-g0) zVal=src[ pixList[p+g0][0] ][ pixList[p+g0][1] ]
		variable zValmax=wavemax(zVal)
		for(j=0;j<dimsize(zVal,0);j+=1)
			if(zValmax==zVal[j])
				break
			endif
		endfor
		pixListMax[ pixList[g0][6] ][0]=pixList[j+g0][0]
		pixListMax[ pixList[g0][6] ][1]=pixList[j+g0][1]
	while(br==0)
	
	//calculate r[mm] and phi[deg]
	variable/c rthe
	for(i=0;i<dimsize(pixListMax,0);i+=1)
		rthe=r2polar(cmplx(dimdelta(src,0)*(pixListMax[i][0]-pc),dimdelta(src,1)*(pixListMax[i][1]-qc)))
		pixListMax[i][2]=real(rthe)
		pixListMax[i][3]=180/pi*imag(rthe)
	endfor
	
	//sort by phi
	duplicate/free/o pixListMax,pixListMax1
	make/free/o/n=(dimsize(pixListMax,0)) plm_phi=pixListMax[p][3],exc=p
	sort,plm_phi,exc
	pixListMax=pixListMax1[exc[p]][q]
	
	//show in the listbox
	wave plm=pixListMax
	wave/t plm_t
	wave select
	redimension/n=(dimsize(plm,0),-1,-1) plm_t,select
	
	for(i=0;i<dimsize(plm,0);i+=1)
		plm_t[i]=getPixPosString(src,plm[i][0],plm[i][1])
		select[i][0][%backColors]=0
	endfor
	select[0][0][0]=1
	
	
	cd $back
	return 0
end
static function getLength(w)
	wave w//区間ウェーブ
	
	variable size=dimsize(w,0)
	variable xb=pnt2x(w,0)
	variable xe=pnt2x(w,size-1)
	variable yb=w[0]
	variable ye=w[size-1]
	
	//全部がベースラインより上
	//ついでに最大値も探す
	variable i,im=0
	for(i=0;i<size;i+=1)
		if(w[i]<LineY(xb,yb,xe,ye,pnt2x(w,i)))
			return -1
		elseif(w[i]>w[im])
			im=i
		endif	
	endfor
	
	variable base=LineY(xb,yb,xe,ye,pnt2x(w,im))
	if(base<=0)
		return -2
	endif
	//最大値のベースラインからの高さ割合(%)
	variable height=(w[im]-base)/base*100
	
	return height
end
static function LineY(x1,y1,x2,y2,xx)
	variable x1,y1,x2,y2,xx
	return (y2-y1)/(x2-x1)*(xx-x1)+y1
end

//隣接ピクセルを見つける
static function/wave findNei(pixList,maxInd,ind)
	wave pixList
	variable maxInd
	variable ind
	
	wave pl=pixList
	variable sz=maxInd
	
	make/o/n=(sz) indice=-1
	indice[0]=ind
	variable pos=1,newpos=pos
	variable index
	variable i,j,k,i0=0
	do
		//indiceにある行番号に対して
		for(i=i0;i<pos;i+=1)
			
			
			//隣接ピクセル番号を確認
			for(j=2;j<=5;j+=1)
				
				index=pl[indice[i]][j]
				
				//正の数なら
				if(index>=0)
					//等しい値がindiceにある?
					for(k=0;k<newpos;k+=1)
						if(indice[k]==index)
							//あればスキップ
							break
						elseif(k==newpos-1)
							//なければ新しい値なので記録してnewposを伸ばす
							indice[newpos]=index
							newpos+=1
						endif
					endfor
				endif
				
			
			endfor // j
			
		endfor // i
		
		//新しく記録されたpos~newposの行番号を確認する
		if(newpos>pos)
			i0=pos
			pos=newpos
		else
			pos=newpos
			break
		endif
		
	while(1)
	
	deletepoints/m=0 pos,dimsize(indice,0)-pos,indice
	
	//print indice
	
	return indice
end

Function SpotFinder_v2_ListBoxProc(lba) : ListBoxControl
	STRUCT WMListboxAction &lba

	Variable row = lba.row
	Variable col = lba.col
	WAVE/T/Z listWave = lba.listWave
	WAVE/Z selWave = lba.selWave

	switch( lba.eventCode )
		case 4: // cell selection		
			//move cursor
			wave plm=$(homedir()+"pixListMax")
			//print row
			if(row<dimsize(plm,0))
				Cursor/i/p/w=$imgwinnam G,''#0,plm[row][0],plm[row][1]
			endif
			
			break
		default:
			break
	endswitch

	return 0
End
