1.
프로젝트->속성->구성속성->VC++ 디렉터리->포함디렉터리
DarknessReFactoring\Common 추가.
directx11 sdk 설치된 폴더내의 include폴더 추가.

2.
프로젝트->속성->구성속성->VC++ 디렉터리->라이브러리 디렉터리
DarknessReFactoring\Common 추가.
directx11 sdk 설치된 폴더내의 lib\x86폴더 추가

3.
프로젝트->속성->구성속성->링커->입력->추가종속성
imm32.lib
d3d11.lib
d3dx11d.lib
D3DCompiler.lib
Effects11d.lib
dxgi.lib
dxguid.lib
kernel32.lib
user32.lib
gdi32.lib
winspool.lib
comdlg32.lib
advapi32.lib
shell32.lib
ole32.lib
oleaut32.lib
uuid.lib
odbc32.lib
odbccp32.lib
추가.
