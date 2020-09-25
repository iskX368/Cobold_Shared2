#pragma rtGlobals=3		// Use modern global access method and strict wave access.


static strconstant procnam="ConvFuncGrapher"
static strconstant folnam="root:About_ConvFuncGrapher"
static strconstant winnam="ConvFuncGrapher"
static strconstant gwinnam="ConvFuncGrapher_XYimage"

static constant cp_dist=80 // [mm]
static strconstant sep=";"



menu "Macros"
	submenu "ConvFuncGrapher"
		"Initialize",ConvFuncGrapher_Init()
		"How to use",ConvFuncGrapher_HowToUse()
	endmacro
endmacro

function ConvFuncGrapher_HowToUse()
	print "-----------------------------------------------------------------------------------------------"
	print "ConvFuncGrapher"
	print "\t検出器XYプロット上に，座標ウェーブXYwaveを返す関数  function/wave conversion(XYwave, theta [deg], phi [deg])  のtheta一定曲線を描く"
	print "\t1. メニューのMacrosからInitializeを選択"
	print "\t2. 計算したい関数を含んだプロシージャウィンドウを選択"
	print "\t3. 変換関数一覧より関数を選択"
	print "\t4. 曲線を描きたいXY plotをリストより選択してイメージを表示"
	print "\t5. thetaまたはhole numberを選択して曲線を移動して確認"
	print "\t6. 線の色やグラフスケールを調節"
	print "作成日: 2019.11.19"
	print "-----------------------------------------------------------------------------------------------"
end

function/wave ConvFuncGrapher_proto(dst,theta_deg,varphi_deg)
	wave dst
	variable theta_deg,varphi_deg
	
	dst[0]=0//x
	dst[1]=0//y
	
	return dst
end


//選択で更新
function/t ConvFuncGrapher_ProcList()
	return removefromlist(procnam,winlist("*",sep,",INDEPENDENTMODULE:0,INCLUDE:1"),sep)
	
end
//選択で更新
function/t ConvFuncGrapher_FuncList()
	
	//change conv func list
	//TODO		どうやってプロトタイプ関数に限定する？
	
	controlInfo/w=$winnam popup0
	string popStr=S_Value
	string condi="KIND:2,NPARAMS:3,VALTYPE:8,WIN:"+popStr
	return functionlist("*",sep,condi)
end
function/t ConvFuncGrapher_XYList()
	return wavelist("*",sep,"DIMS:2,CMPLX:0,TEXT:0,WAVE:0")
end



Function ConvFuncGrapher_PopMenuProc(pa) : PopupMenuControl
	STRUCT WMPopupAction &pa

	switch( pa.eventCode )
		case 2: // mouse up
			Variable popNum = pa.popNum
			String popStr = pa.popStr
			
			if(cmpstr("popup0",pa.ctrlName)==0)//ProcWin
				popupmenu popup1,mode=1,popvalue=StringFromList(0,ConvFuncGrapher_FuncList(),sep)
			elseif(cmpstr("popup2",pa.ctrlName)==0)//XY image
				
				//duplicate source
				wave src0=$popStr
				if(!waveexists(src0))
					print "XY image wave not found. "
					return 1
				endif
				duplicate/o src0,$(folnam+":source")/wave=src
				
				//get wave max
				variable/g $(folnam+":gv_wavemax")=wavemax2(src)
				
				//Set pix list
				SetContourPixList()
				
				//put contour
				wave dst=putContour()
				
				//draw
				dowindow/f $gwinnam
				if(!V_Flag)
					PauseUpdate; Silent 1		// building window...
					Display /W=(810,61.25,1204.5,425.75)/N=$gwinnam/K=1
					AppendImage dst
					ModifyImage ''#0 ctab= {*,*,PlanetEarth,0}
					ModifyGraph mirror=2
					ModifyGraph height={Aspect,1}
					SetAxis left -35,35
					SetAxis bottom -35,35
					
					getwindow $winnam,wsizeOuter
					variable l0=V_right
					variable t0=V_top
					dowindow/f $gwinnam
					getwindow $gwinnam,wsize
					variable wid=V_right-V_left
					variable hei=V_bottom-V_top
					movewindow/w=$gwinnam l0,t0,l0+wid,t0+hei
				endif
				
			elseif(cmpstr("popup3",pa.ctrlName)==0)//scale
				dowindow/f $gwinnam
				if(V_Flag)
					
					nvar gv_wavemax=$(folnam+":gv_wavemax")
					variable scale=-1
					if(cmpstr("Auto",popStr)==0)
						scale=-1
					elseif(cmpstr("75%",popStr)==0)
						scale=0.75
					elseif(cmpstr("50%",popStr)==0)
						scale=0.5
					elseif(cmpstr("20%",popStr)==0)
						scale=0.2
					elseif(cmpstr("5%",popStr)==0)
						scale=0.05
					elseif(cmpstr("1%",popStr)==0)
						scale=0.01
					else
						scale=-1
					endif
					
					if(scale<0)
						ModifyImage/w=$gwinnam ''#0 ctab= {*,*,PlanetEarth,0}
					else
						ModifyImage/w=$gwinnam ''#0 ctab= {*,round(gv_wavemax*scale),PlanetEarth,0}
					endif
				endif
				
			endif
			
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End

static function SetContourPixList()
	
	//source
	wave w=$(folnam+":source")
	if(!waveexists(w))
		print "make source"
		return 1
	endif
	
	//get theta
	controlinfo/w=$winnam setvar0
	variable theta_deg=V_Value
	
	
	//get function
	controlinfo/w=$winnam popup1
	string sf=S_Value
	funcref ConvFuncGrapher_proto f=$sf			
	
	variable x0=dimoffset(w,0)
	variable y0=dimoffset(w,1)
	variable dx=dimdelta(w,0)
	variable dy=dimdelta(w,1)
	variable sizex=dimsize(w,0)
	variable sizey=dimsize(w,1)
	
	make/free/o ConvFuncGrapher_coord
	
	make/o/n=(sizex*sizey,2) $(folnam+":pixlist")/wave=dst
	
	//find pixels
	variable i,j,cnt=0
	variable phid
	for(phid=0;phid<360;phid+=1)
		f(ConvFuncGrapher_coord,theta_deg,phid)
		i=round((ConvFuncGrapher_coord[0]-x0)/dx)
		j=round((ConvFuncGrapher_coord[1]-y0)/dy)
		if(i>=0 && i<sizex && j>=0 && j<sizey)
			dst[cnt][0]=i
			dst[cnt][1]=j
			cnt+=1
		endif
	endfor
	
	deletepoints/m=0 cnt,dimsize(dst,0)-cnt,dst
	
end

static function/wave putContour()
	
	make/free/o ConvFuncGrapher_coord
	
	
	//get line color
	controlinfo/w=$winnam setvar2
	variable linecolor=V_Value
	
	//get scale
	controlInfo/w=$winnam popup3
	string popStr=S_Value
	variable scale=-1
	if(cmpstr("Auto",popStr)==0)
		scale=-1
	elseif(cmpstr("75%",popStr)==0)
		scale=0.75
	elseif(cmpstr("50%",popStr)==0)
		scale=0.5
	elseif(cmpstr("20%",popStr)==0)
		scale=0.2
	elseif(cmpstr("5%",popStr)==0)
		scale=0.05
	elseif(cmpstr("1%",popStr)==0)
		scale=0.01
	else
		scale=-1
	endif
	
	//get wavemax2
	nvar gv_wavemax=$(folnam+":gv_wavemax")
	variable val=(scale>0) ? gv_wavemax*scale*linecolor : gv_wavemax*linecolor
	
	//get pixlist
	wave pl=$(folnam+":pixlist")
	
	//duplicate
	wave src=$(folnam+":source")
	duplicate/o src,$(folnam+":XYimage")/wave=dst
	
	//color
	variable i
	for(i=0;i<dimsize(pl,0);i+=1)
		dst[ pl[i][0] ][ pl[i][1] ]=val
	endfor

	return dst
end
static function wavemax2(w)
	wave w
	
	variable ans=-inf
	variable i,j
	for(i=0;i<dimsize(w,0);i+=1)
		for(j=0;j<dimsize(w,1);j+=1)
			ans=(w[i][j]>ans) ? w[i][j] : ans
		endfor
	endfor
	
	return ans
end


Function ConvFuncGrapher_SetVarProc(sva) : SetVariableControl
	STRUCT WMSetVariableAction &sva

	switch( sva.eventCode )
		case 1: // mouse up
		case 2: // Enter key
		case 3: // Live update
			Variable dval = sva.dval
			String sval = sva.sval
			
			if(cmpstr("setvar0",sva.ctrlName)==0)//theta [deg]
				SetContourPixList()
				putContour()
			elseif(cmpstr("setvar1",sva.ctrlName)==0)//Hole number
				variable theta_deg=atan(2*dval/cp_dist)*180/pi
				
				nvar theta_deg_=$(folnam+":gv_theta_deg")
				theta_deg_=atan(2*dval/cp_dist)*180/pi
				
				SetContourPixList()
				putContour()
			elseif(cmpstr("setvar2",sva.ctrlName)==0)//color
				putContour()				
			else
			endif
			
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End

function ConvFuncGrapher_WinHook(s)
	STRUCT WMWinHookStruct &s
	
	Variable hookResult = 0

	switch(s.eventCode)
		case 3:				// mouse down
			// Handle activate
			dowindow $gwinnam
			if(V_Flag)
				getwindow kwtopwin,wsizeOuter
				variable l0=V_right
				variable t0=V_top
				dowindow/f $gwinnam
				getwindow $gwinnam,wsize
				variable wid=V_right-V_left
				variable hei=V_bottom-V_top
				movewindow/w=$gwinnam l0,t0,l0+wid,t0+hei
				dowindow/f $winnam
			endif
			hookResult = 1
			
			break
		case 2:				// kill
			dowindow $gwinnam
			if(V_Flag)
				killwindow $gwinnam
			endif
			if(datafolderexists(folnam))
				killdatafolder $folnam
			endif
						
			hookResult=1
			break
		default:
			hookResult=0
			break
	endswitch

	return hookResult		// 0 if nothing done, else 1
end

function ConvFuncGrapher_Init()
	
	string back=getdatafolder(1)
	
	if(datafolderexists(folnam))
		killdatafolder $folnam
	endif
	newdatafolder/o/s $folnam
	
	make/o/n=(1,1) XYimage=0
	variable/g gv_theta_deg=0
	
	dowindow $winnam
	if(V_Flag)
		killwindow $winnam
	endif
	dowindow $gwinnam
	if(V_Flag)
		killwindow $gwinnam
	endif
	//
	PauseUpdate; Silent 1		// building window...
	NewPanel /W=(244,170,532,336)/K=1
	ShowTools/A
	PopupMenu popup0,pos={9,7},size={153,20},proc=ConvFuncGrapher_PopMenuProc,title="Procedure Window"
	PopupMenu popup0,mode=2,popvalue="Proc0",value= #"ConvFuncGrapher_ProcList()"
	PopupMenu popup1,pos={9,34},size={160,20},proc=ConvFuncGrapher_PopMenuProc,title="Conversion Function"
	PopupMenu popup1,mode=1,popvalue="abcd",value= #"ConvFuncGrapher_FuncList()"
	PopupMenu popup2,pos={10,63},size={160,20},proc=ConvFuncGrapher_PopMenuProc,title="XY image"
	PopupMenu popup2,mode=6,popvalue="e19111402_XYel",value= #"ConvFuncGrapher_XYList()"
	SetVariable setvar0,pos={10,117},size={128,16},proc=ConvFuncGrapher_SetVarProc,title="Theta [deg] "
	SetVariable setvar0,limits={0,20,0.5},value= root:About_ConvFuncGrapher:gv_theta_deg
	SetVariable setvar1,pos={9,135},size={129,16},proc=ConvFuncGrapher_SetVarProc,title="Hole number"
	SetVariable setvar1,limits={0,15,1},value= _NUM:10
	SetVariable setvar2,pos={153,117},size={90,16},proc=ConvFuncGrapher_SetVarProc,title="line color"
	SetVariable setvar2,limits={0,1,0.1},value= _NUM:0.3
	PopupMenu popup3,pos={168,135},size={82,20},proc=ConvFuncGrapher_PopMenuProc,title="scale"
	PopupMenu popup3,mode=1,popvalue="Auto",value= #"\"Auto;75%;50%;20%;5%;1%\""
	ToolsGrid snap=1,visible=1
	SetWindow kwTopWin,hook(hook)=ConvFuncGrapher_WinHook
	//
	
	
	dowindow/c $winnam
	PopupMenu popup0,value=#"ConvFuncGrapher_ProcList()"
	PopupMenu popup1,value=#"ConvFuncGrapher_FuncList()"
	PopupMenu popup2,value=#"ConvFuncGrapher_XYList()"
	PopupMenu popup0,mode=1,popvalue=StringFromList(0,ConvFuncGrapher_ProcList(),sep)
	PopupMenu popup1,mode=1,popvalue=StringFromList(0,ConvFuncGrapher_FuncList(),sep)
	PopupMenu popup2,mode=1,popvalue=StringFromList(0,ConvFuncGrapher_XYList(),sep)
		
	setwindow $winnam hook(hook)=ConvFuncGrapher_WinHook
	SetVariable setvar0,limits={0,20,0.5},value=gv_theta_deg
	
	
	cd $back
end


Window ConvFuncGrapher() : Panel
	PauseUpdate; Silent 1		// building window...
	NewPanel /W=(244,170,532,336)
	ShowTools/A
	PopupMenu popup0,pos={9,7},size={153,20},proc=ConvFuncGrapher_PopMenuProc,title="Procedure Window"
	PopupMenu popup0,mode=2,popvalue="Proc0",value= #"ConvFuncGrapher_ProcList()"
	PopupMenu popup1,pos={9,34},size={160,20},proc=ConvFuncGrapher_PopMenuProc,title="Conversion Function"
	PopupMenu popup1,mode=1,popvalue="abcd",value= #"ConvFuncGrapher_FuncList()"
	PopupMenu popup2,pos={10,63},size={160,20},proc=ConvFuncGrapher_PopMenuProc,title="XY image"
	PopupMenu popup2,mode=6,popvalue="e19111402_XYel",value= #"ConvFuncGrapher_XYList()"
	SetVariable setvar0,pos={10,117},size={128,16},proc=ConvFuncGrapher_SetVarProc,title="Theta [deg] "
	SetVariable setvar0,limits={0,20,0.5},value= root:About_ConvFuncGrapher:gv_theta_deg
	SetVariable setvar1,pos={9,135},size={129,16},proc=ConvFuncGrapher_SetVarProc,title="Hole number"
	SetVariable setvar1,limits={0,15,1},value= _NUM:10
	SetVariable setvar2,pos={153,117},size={90,16},proc=ConvFuncGrapher_SetVarProc,title="line color"
	SetVariable setvar2,limits={0,1,0.1},value= _NUM:0.3
	PopupMenu popup3,pos={168,135},size={82,20},proc=ConvFuncGrapher_PopMenuProc,title="scale"
	PopupMenu popup3,mode=1,popvalue="Auto",value= #"\"Auto;75%;50%;20%;5%;1%\""
	ToolsGrid snap=1,visible=1
	SetWindow kwTopWin,hook(hook)=ConvFuncGrapher_WinHook
EndMacro

Window Graph0() : Graph
	PauseUpdate; Silent 1		// building window...
	Display /W=(810,61.25,1204.5,425.75)
	AppendImage e19111401_XYel
	ModifyImage e19111401_XYel ctab= {*,*,PlanetEarth,0}
	ModifyGraph mirror=2
	SetAxis left -35,35
	SetAxis bottom -35,35
EndMacro
