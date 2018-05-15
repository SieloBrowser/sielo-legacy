/****************************************************************************
** Meta object code from reading C++ file 'AdBlockPage.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/Preferences/AdBlockPage.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AdBlockPage.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__AdBlockPage_t {
    QByteArrayData data[16];
    char stringdata0[186];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__AdBlockPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__AdBlockPage_t qt_meta_stringdata_Sn__AdBlockPage = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Sn::AdBlockPage"
QT_MOC_LITERAL(1, 16, 7), // "addRule"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 10), // "removeRule"
QT_MOC_LITERAL(4, 36, 15), // "addSubscription"
QT_MOC_LITERAL(5, 52, 18), // "removeSubscription"
QT_MOC_LITERAL(6, 71, 14), // "currentChanged"
QT_MOC_LITERAL(7, 86, 5), // "index"
QT_MOC_LITERAL(8, 92, 12), // "filterString"
QT_MOC_LITERAL(9, 105, 6), // "string"
QT_MOC_LITERAL(10, 112, 13), // "enableAdBlock"
QT_MOC_LITERAL(11, 126, 5), // "state"
QT_MOC_LITERAL(12, 132, 15), // "aboutToShowMenu"
QT_MOC_LITERAL(13, 148, 15), // "learnAboutRules"
QT_MOC_LITERAL(14, 164, 16), // "loadSubscription"
QT_MOC_LITERAL(15, 181, 4) // "load"

    },
    "Sn::AdBlockPage\0addRule\0\0removeRule\0"
    "addSubscription\0removeSubscription\0"
    "currentChanged\0index\0filterString\0"
    "string\0enableAdBlock\0state\0aboutToShowMenu\0"
    "learnAboutRules\0loadSubscription\0load"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__AdBlockPage[] = {

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
       1,    0,   69,    2, 0x08 /* Private */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    0,   71,    2, 0x08 /* Private */,
       5,    0,   72,    2, 0x08 /* Private */,
       6,    1,   73,    2, 0x08 /* Private */,
       8,    1,   76,    2, 0x08 /* Private */,
      10,    1,   79,    2, 0x08 /* Private */,
      12,    0,   82,    2, 0x08 /* Private */,
      13,    0,   83,    2, 0x08 /* Private */,
      14,    0,   84,    2, 0x08 /* Private */,
      15,    0,   85,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Sn::AdBlockPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AdBlockPage *_t = static_cast<AdBlockPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addRule(); break;
        case 1: _t->removeRule(); break;
        case 2: _t->addSubscription(); break;
        case 3: _t->removeSubscription(); break;
        case 4: _t->currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->filterString((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->enableAdBlock((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->aboutToShowMenu(); break;
        case 8: _t->learnAboutRules(); break;
        case 9: _t->loadSubscription(); break;
        case 10: _t->load(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::AdBlockPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__AdBlockPage.data,
      qt_meta_data_Sn__AdBlockPage,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::AdBlockPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::AdBlockPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__AdBlockPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::AdBlockPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
