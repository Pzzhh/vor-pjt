set filename=VOR
set path=OBJ
::��ȡ���� ����ʽ����Ϊ��20110820
set datevar=%date:~0,4%%date:~5,2%%date:~8,2%
set timevar=%time:~0,2%
if /i %timevar% LSS 10 (
set timevar=0%time:~1,1%
)
::��ȡʱ���еķ֡��� ����ʽ����Ϊ��3220 ����ʾ 32��20��
set timevar=%timevar%%time:~3,2%%time:~6,2%
set copyfilename=%filename%-%datevar%-%timevar%
del ..\*.hex
copy ..\%path%\template.hex ..\%copyfilename%.hex

@echo filename:%copyfilename%.hex
