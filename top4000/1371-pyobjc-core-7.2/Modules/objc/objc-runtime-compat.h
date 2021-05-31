#ifndef PyObjC_RUNTIME_COMPAT
#define PyObjC_RUNTIME_COMPAT
/*
 * Objective-C 2.x runtime compatibility.
 *
 * This files makes it possible to use the Objective-C 2.x API on versions
 * of Mac OS X before 10.5.
 *
 * Special:
 *  - PyObjC_class_addMethodList is not a function in the ObjC 2.0 runtime API,
 *    but added here to (a) get semantics that are slightly nicer for what
 *    we do and (b) can be implemented efficiently on the "1.0" runtime.
 *  - Modifying a created but not yet registered class should be done using
 *    the preclass_* functions, not the regular ones because it isn't possible
 *    to emulate the entire ObjC 2.0 API on Tiger.
 */
#include <objc/Protocol.h>
#include <objc/objc-runtime.h>

struct PyObjC_method {
    SEL         name;
    IMP         imp;
    const char* type;
};


#define _C_IN 'n'
#define _C_INOUT 'N'
#define _C_OUT 'o'
#define _C_ONEWAY 'V'
#define _C_BYCOPY 'O'
#define _C_BYREF 'R'

/* These don't actually exist in the Objective-C runtime, but are used
 * by the bridge to simplify code.
 */
#define _C_UNICHAR 'T'
#define _C_CHAR_AS_TEXT 't'
#define _C_CHAR_AS_INT 'z'
#define _C_NSBOOL 'Z'

#define objc_superSetReceiver(super, val) (super).receiver = (val)
#define objc_superGetReceiver(super) ((super).receiver)

#define objc_superSetClass(super, cls) (super).super_class = (cls)
#define objc_superGetClass(super) ((super).super_class)


/* Some functions that are missing (oddly enough) */
BOOL PyObjC_class_isSubclassOf(Class child, Class parent);
#define class_isSubclassOf PyObjC_class_isSubclassOf

/*
 * XXX: Override protocol_getMethodDescription. This is a crude hack that's added because
 * protocol_getMethodDescription sometimes gives the wrong answer (test_protocols.py).
 * I haven't found the root cause for this yet, it may or may not be a problem with
 * PyObjC.
 */
extern struct objc_method_description
PyObjC_protocol_getMethodDescription(Protocol* p, SEL aSel, BOOL isRequiredMethod,
                                     BOOL isInstanceMethod);
#define protocol_getMethodDescription PyObjC_protocol_getMethodDescription


extern BOOL PyObjC_class_addMethodList(Class, struct PyObjC_method*, unsigned int);

#endif /* PyObjC_RUNTIME_COMPAT */
