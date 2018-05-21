/****************************************************************************
** Meta object code from reading C++ file 'CookieManager.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Cookies/CookieManager.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CookieManager.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__CookieManager_t {
    QByteArrayData data[19];
    char stringdata0[224];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__CookieManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__CookieManager_t qt_meta_stringdata_Sn__CookieManager = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Sn::CookieManager"
QT_MOC_LITERAL(1, 18, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 55, 7), // "current"
QT_MOC_LITERAL(5, 63, 6), // "parent"
QT_MOC_LITERAL(6, 70, 6), // "remove"
QT_MOC_LITERAL(7, 77, 9), // "removeAll"
QT_MOC_LITERAL(8, 87, 12), // "addWhiteList"
QT_MOC_LITERAL(9, 100, 15), // "removeWhiteList"
QT_MOC_LITERAL(10, 116, 12), // "addBlackList"
QT_MOC_LITERAL(11, 129, 15), // "removeBlackList"
QT_MOC_LITERAL(12, 145, 13), // "deletePressed"
QT_MOC_LITERAL(13, 159, 12), // "filterString"
QT_MOC_LITERAL(14, 172, 6), // "string"
QT_MOC_LITERAL(15, 179, 9), // "addCookie"
QT_MOC_LITERAL(16, 189, 14), // "QNetworkCookie"
QT_MOC_LITERAL(17, 204, 6), // "cookie"
QT_MOC_LITERAL(18, 211, 12) // "removeCookie"

    },
    "Sn::CookieManager\0currentItemChanged\0"
    "\0QTreeWidgetItem*\0current\0parent\0"
    "remove\0removeAll\0addWhiteList\0"
    "removeWhiteList\0addBlackList\0"
    "removeBlackList\0deletePressed\0"
    "filterString\0string\0addCookie\0"
    "QNetworkCookie\0cookie\0removeCookie"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__CookieManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   69,    2, 0x08 /* Private */,
       6,    0,   74,    2, 0x08 /* Private */,
       7,    0,   75,    2, 0x08 /* Private */,
       8,    0,   76,    2, 0x08 /* Private */,
       9,    0,   77,    2, 0x08 /* Private */,
      10,    0,   78,    2, 0x08 /* Private */,
      11,    0,   79,    2, 0x08 /* Private */,
      12,    0,   80,    2, 0x08 /* Private */,
      13,    1,   81,    2, 0x08 /* Private */,
      15,    1,   84,    2, 0x08 /* Private */,
      18,    1,   87,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   17,

       0        // eod
};

void Sn::CookieManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CookieManager *_t = static_cast<CookieManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->remove(); break;
        case 2: _t->removeAll(); break;
        case 3: _t->addWhiteList(); break;
        case 4: _t->removeWhiteList(); break;
        case 5: _t->addBlackList(); break;
        case 6: _t->removeBlackList(); break;
        case 7: _t->deletePressed(); break;
        case 8: _t->filterString((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->addCookie((*reinterpret_cast< const QNetworkCookie(*)>(_a[1]))); break;
        case 10: _t->removeCookie((*reinterpret_cast< const QNetworkCookie(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkCookie >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkCookie >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::CookieManager::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Sn__CookieManager.data,
      qt_meta_data_Sn__CookieManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::CookieManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::CookieManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__CookieManager.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Sn::CookieManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
