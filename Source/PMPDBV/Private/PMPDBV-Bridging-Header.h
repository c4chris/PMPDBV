//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#ifdef _MSC_VER
#define _Nonnull
#define _Nullable
#endif

typedef struct Space_Point Point3D;
struct Space_Point
{
  double x, y, z;
};

#if defined(_MSC_VER) || defined(__ANDROID__)
typedef bool Boolean;
#else
typedef _Bool Boolean;
#endif
typedef double DBL;

typedef struct FSpace_Point FPoint3D;
struct FSpace_Point
{
  float x, y, z;
};

extern char * _Nullable bundleRsrcDir;
extern char * _Nullable downloadDir;
extern char * _Nullable tempDir;

struct SwiftCallbacks
{
  void (*_Nonnull GL_atm)(FPoint3D A, short layer, unsigned short group, unsigned short atom, char chain, long color);
  void (*_Nonnull GL_bnd)(FPoint3D A, FPoint3D B, short layer, unsigned short group, unsigned short atomA, unsigned short atomB, char chain, long color);
  void (*_Nonnull GL_Atom_Color)(char * _Nonnull name, double red, double green, double blue, Boolean isMetallic);
  void (*_Nonnull GL_All_Invisible)(short layer);
  void (*_Nonnull GL_ForceRebuild)(short layer);
  void (*_Nonnull GL_New_Shape)(void);
  void (*_Nonnull GL_Add_Vertex)(Point3D V, Point3D N, unsigned int color);
  void (*_Nonnull GL_Draw_Shape)(short layer, char type);
};
typedef struct SwiftCallbacks SwiftCallbacks;

extern void SwiftCBSetup(const SwiftCallbacks *_Nonnull callbacks);
extern void GL_init_Atom_Colors(void);
void GLRender(void);
short InitGlobals(void);
void doPDBinput(const char * _Nullable, const char * _Nullable);

extern int (*_Nullable printfDelegate)(const char* fmt, va_list va);
