/****************************************************************************
** Meta object code from reading C++ file 'DelayedFileWatcher.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Utils/DelayedFileWatcher.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DelayedFileWatcher.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__DelayedFileWatcher_t {
    QByteArrayData data[5];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__DelayedFileWatcher_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__DelayedFileWatcher_t qt_meta_stringdata_Sn__DelayedFileWatcher = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Sn::DelayedFileWatcher"
QT_MOC_LITERAL(1, 23, 23), // "delayedDirectoryChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 4), // "path"
QT_MOC_LITERAL(4, 53, 18) // "delayedFileChanged"

    },
    "Sn::DelayedFileWatcher\0delayedDirectoryChanged\0"
    "\0path\0delayedFileChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__DelayedFileWatcher[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       4,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Sn::DelayedFileWatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DelayedFileWatcher *_t = static_cast<DelayedFileWatcher *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->delayedDirectoryChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->delayedFileChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (DelayedFileWatcher::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DelayedFileWatcher::delayedDirectoryChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DelayedFileWatcher::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DelayedFileWatcher::delayedFileChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::DelayedFileWatcher::staticMetaObject = {
    { &QFileSystemWatcher::staticMetaObject, qt_meta_stringdata_Sn__DelayedFileWatcher.data,
      qt_meta_data_Sn__DelayedFileWatcher,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::DelayedFileWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::DelayedFileWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__DelayedFileWatcher.stringdata0))
        return static_cast<void*>(this);
    return QFileSystemWatcher::qt_metacast(_clname);
}

int Sn::DelayedFileWatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFileSystemWatcher::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Sn::DelayedFileWatcher::delayedDirectoryChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Sn::DelayedFileWatcher::delayedFileChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
