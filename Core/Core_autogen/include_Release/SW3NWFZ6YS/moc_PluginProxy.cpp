/****************************************************************************
** Meta object code from reading C++ file 'PluginProxy.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Plugins/PluginProxy.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PluginProxy.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__PluginProxy_t {
    QByteArrayData data[13];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__PluginProxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__PluginProxy_t qt_meta_stringdata_Sn__PluginProxy = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Sn::PluginProxy"
QT_MOC_LITERAL(1, 16, 14), // "webPageCreated"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 8), // "WebPage*"
QT_MOC_LITERAL(4, 41, 4), // "page"
QT_MOC_LITERAL(5, 46, 14), // "webPageDeleted"
QT_MOC_LITERAL(6, 61, 17), // "mainWindowCreated"
QT_MOC_LITERAL(7, 79, 11), // "MainWindow*"
QT_MOC_LITERAL(8, 91, 6), // "window"
QT_MOC_LITERAL(9, 98, 17), // "mainWindowDeleted"
QT_MOC_LITERAL(10, 116, 14), // "pluginUnloaded"
QT_MOC_LITERAL(11, 131, 16), // "PluginInterface*"
QT_MOC_LITERAL(12, 148, 6) // "plugin"

    },
    "Sn::PluginProxy\0webPageCreated\0\0"
    "WebPage*\0page\0webPageDeleted\0"
    "mainWindowCreated\0MainWindow*\0window\0"
    "mainWindowDeleted\0pluginUnloaded\0"
    "PluginInterface*\0plugin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__PluginProxy[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,
       6,    1,   45,    2, 0x06 /* Public */,
       9,    1,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,

       0        // eod
};

void Sn::PluginProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PluginProxy *_t = static_cast<PluginProxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->webPageCreated((*reinterpret_cast< WebPage*(*)>(_a[1]))); break;
        case 1: _t->webPageDeleted((*reinterpret_cast< WebPage*(*)>(_a[1]))); break;
        case 2: _t->mainWindowCreated((*reinterpret_cast< MainWindow*(*)>(_a[1]))); break;
        case 3: _t->mainWindowDeleted((*reinterpret_cast< MainWindow*(*)>(_a[1]))); break;
        case 4: _t->pluginUnloaded((*reinterpret_cast< PluginInterface*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (PluginProxy::*_t)(WebPage * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PluginProxy::webPageCreated)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (PluginProxy::*_t)(WebPage * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PluginProxy::webPageDeleted)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (PluginProxy::*_t)(MainWindow * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PluginProxy::mainWindowCreated)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (PluginProxy::*_t)(MainWindow * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PluginProxy::mainWindowDeleted)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::PluginProxy::staticMetaObject = {
    { &Plugins::staticMetaObject, qt_meta_stringdata_Sn__PluginProxy.data,
      qt_meta_data_Sn__PluginProxy,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::PluginProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::PluginProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__PluginProxy.stringdata0))
        return static_cast<void*>(this);
    return Plugins::qt_metacast(_clname);
}

int Sn::PluginProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Plugins::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Sn::PluginProxy::webPageCreated(WebPage * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Sn::PluginProxy::webPageDeleted(WebPage * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Sn::PluginProxy::mainWindowCreated(MainWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Sn::PluginProxy::mainWindowDeleted(MainWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
