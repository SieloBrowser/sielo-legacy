/****************************************************************************
** Meta object code from reading C++ file 'Appearance.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/Preferences/Appearance.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Appearance.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__AppearancePage_t {
    QByteArrayData data[13];
    char stringdata0[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__AppearancePage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__AppearancePage_t qt_meta_stringdata_Sn__AppearancePage = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Sn::AppearancePage"
QT_MOC_LITERAL(1, 19, 12), // "loadSettings"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 14), // "currentChanged"
QT_MOC_LITERAL(4, 48, 11), // "showLicense"
QT_MOC_LITERAL(5, 60, 29), // "tabsSpacesPaddingValueChanged"
QT_MOC_LITERAL(6, 90, 5), // "value"
QT_MOC_LITERAL(7, 96, 25), // "backgroundLocationClicked"
QT_MOC_LITERAL(8, 122, 11), // "openGallery"
QT_MOC_LITERAL(9, 134, 8), // "addTheme"
QT_MOC_LITERAL(10, 143, 8), // "getColor"
QT_MOC_LITERAL(11, 152, 21), // "useRealToolBarChanged"
QT_MOC_LITERAL(12, 174, 7) // "enabled"

    },
    "Sn::AppearancePage\0loadSettings\0\0"
    "currentChanged\0showLicense\0"
    "tabsSpacesPaddingValueChanged\0value\0"
    "backgroundLocationClicked\0openGallery\0"
    "addTheme\0getColor\0useRealToolBarChanged\0"
    "enabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__AppearancePage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    1,   62,    2, 0x08 /* Private */,
       7,    0,   65,    2, 0x08 /* Private */,
       8,    0,   66,    2, 0x08 /* Private */,
       9,    0,   67,    2, 0x08 /* Private */,
      10,    0,   68,    2, 0x08 /* Private */,
      11,    1,   69,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   12,

       0        // eod
};

void Sn::AppearancePage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AppearancePage *_t = static_cast<AppearancePage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadSettings(); break;
        case 1: _t->currentChanged(); break;
        case 2: _t->showLicense(); break;
        case 3: _t->tabsSpacesPaddingValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->backgroundLocationClicked(); break;
        case 5: _t->openGallery(); break;
        case 6: _t->addTheme(); break;
        case 7: _t->getColor(); break;
        case 8: _t->useRealToolBarChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::AppearancePage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__AppearancePage.data,
      qt_meta_data_Sn__AppearancePage,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::AppearancePage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::AppearancePage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__AppearancePage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::AppearancePage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
