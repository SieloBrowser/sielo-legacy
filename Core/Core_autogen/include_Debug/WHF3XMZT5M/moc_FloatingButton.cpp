/****************************************************************************
** Meta object code from reading C++ file 'FloatingButton.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/FloatingButton.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FloatingButton.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__FloatingButton_t {
    QByteArrayData data[4];
    char stringdata0[44];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__FloatingButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__FloatingButton_t qt_meta_stringdata_Sn__FloatingButton = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Sn::FloatingButton"
QT_MOC_LITERAL(1, 19, 9), // "isClicked"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 13) // "statusChanged"

    },
    "Sn::FloatingButton\0isClicked\0\0"
    "statusChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__FloatingButton[] = {

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
       1,    0,   24,    2, 0x06 /* Public */,
       3,    0,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Sn::FloatingButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FloatingButton *_t = static_cast<FloatingButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->isClicked(); break;
        case 1: _t->statusChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (FloatingButton::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FloatingButton::isClicked)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (FloatingButton::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FloatingButton::statusChanged)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Sn::FloatingButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_Sn__FloatingButton.data,
      qt_meta_data_Sn__FloatingButton,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::FloatingButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::FloatingButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__FloatingButton.stringdata0))
        return static_cast<void*>(this);
    return QPushButton::qt_metacast(_clname);
}

int Sn::FloatingButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
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
void Sn::FloatingButton::isClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Sn::FloatingButton::statusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_Sn__RootFloatingButton_t {
    QByteArrayData data[7];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__RootFloatingButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__RootFloatingButton_t qt_meta_stringdata_Sn__RootFloatingButton = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Sn::RootFloatingButton"
QT_MOC_LITERAL(1, 23, 14), // "patternChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 7), // "Pattern"
QT_MOC_LITERAL(4, 47, 10), // "newPattern"
QT_MOC_LITERAL(5, 58, 13), // "statusChanged"
QT_MOC_LITERAL(6, 72, 13) // "changePattern"

    },
    "Sn::RootFloatingButton\0patternChanged\0"
    "\0Pattern\0newPattern\0statusChanged\0"
    "changePattern"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__RootFloatingButton[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    0,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   33,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Sn::RootFloatingButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RootFloatingButton *_t = static_cast<RootFloatingButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->patternChanged((*reinterpret_cast< Pattern(*)>(_a[1]))); break;
        case 1: _t->statusChanged(); break;
        case 2: _t->changePattern(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (RootFloatingButton::*_t)(Pattern );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RootFloatingButton::patternChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RootFloatingButton::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RootFloatingButton::statusChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::RootFloatingButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_Sn__RootFloatingButton.data,
      qt_meta_data_Sn__RootFloatingButton,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::RootFloatingButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::RootFloatingButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__RootFloatingButton.stringdata0))
        return static_cast<void*>(this);
    return QPushButton::qt_metacast(_clname);
}

int Sn::RootFloatingButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Sn::RootFloatingButton::patternChanged(Pattern _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Sn::RootFloatingButton::statusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
