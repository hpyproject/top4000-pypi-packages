/*!
 * @header   proxy-registry.h
 * @abstract Maintain a registry of proxied objects
 * @discussion
 *     Object-identity is important in several Cocoa API's. For that
 *     reason we need to make sure that at most one proxy object is
 *     alive for every Python or Objective-C object.
 */
#ifndef PyObjC_PROXY_REGISTRY_H
#define PyObjC_PROXY_REGISTRY_H

extern int PyObjC_weakref_proxy_registry;

extern int PyObjC_InitProxyRegistry(void);

extern int PyObjC_RegisterPythonProxy(id original, PyObject* proxy);
extern int PyObjC_RegisterObjCProxy(PyObject* original, id proxy);

extern void PyObjC_UnregisterPythonProxy(id original, PyObject* proxy);
extern void PyObjC_UnregisterObjCProxy(PyObject* original, id proxy);

extern id        PyObjC_FindObjCProxy(PyObject* original);
extern PyObject* PyObjC_FindPythonProxy(id original);

extern id PyObjC_FindOrRegisterObjCProxy(PyObject* original, id proxy);

#endif /* PyObjC_PROXY_REGISTRY_H */
