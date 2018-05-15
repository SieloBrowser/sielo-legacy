/****************************************************************************
** Meta object code from reading C++ file 'Subscription.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../AdBlock/Subscription.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Subscription.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__ADB__Subscription_t {
    QByteArrayData data[8];
    char stringdata0[127];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__ADB__Subscription_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__ADB__Subscription_t qt_meta_stringdata_Sn__ADB__Subscription = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Sn::ADB::Subscription"
QT_MOC_LITERAL(1, 22, 19), // "subscriptionChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 19), // "subscriptionUpdated"
QT_MOC_LITERAL(4, 63, 17), // "subscriptionError"
QT_MOC_LITERAL(5, 81, 3), // "msg"
QT_MOC_LITERAL(6, 85, 18), // "updateSubscription"
QT_MOC_LITERAL(7, 104, 22) // "subscriptionDownloaded"

    },
    "Sn::ADB::Subscription\0subscriptionChanged\0"
    "\0subscriptionUpdated\0subscriptionError\0"
    "msg\0updateSubscription\0subscriptionDownloaded"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__ADB__Subscription[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    1,   41,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   44,    2, 0x0a /* Public */,
       7,    0,   45,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Sn::ADB::Subscription::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Subscription *_t = static_cast<Subscription *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->subscriptionChanged(); break;
        case 1: _t->subscriptionUpdated(); break;
        case 2: _t->subscriptionError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->updateSubscription(); break;
        case 4: _t->subscriptionDownloaded(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Subscription::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Subscription::subscriptionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Subscription::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Subscription::subscriptionUpdated)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Subscription::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Subscription::subscriptionError)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::ADB::Subscription::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Sn__ADB__Subscription.data,
      qt_meta_data_Sn__ADB__Subscription,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::ADB::Subscription::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::ADB::Subscription::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__ADB__Subscription.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Sn::ADB::Subscription::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void Sn::ADB::Subscription::subscriptionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Sn::ADB::Subscription::subscriptionUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Sn::ADB::Subscription::subscriptionError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
