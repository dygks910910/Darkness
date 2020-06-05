#pragma once

#define IF_NOTX_RTFALSE(x) if(!x)return false;
#define SAFE_DELETE(x) if(x){delete x;x=nullptr;};
#define SAFE_DELETE_ARR(x) if(x){delete[] x;x=nullptr;};
#define SAFE_DELETE_SHUTDOWN(x) if(x){x->Shutdown();SAFE_DELETE(x);}

//d3d
#define SAFE_RELEASE_D3D(x) if(x)x->Release();x = nullptr;
#define IF_FAILED_RTFALSE(x) if(FAILED(x))return false;