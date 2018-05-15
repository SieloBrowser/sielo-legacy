/****************************************************************************
** Meta object code from reading C++ file 'GeneralPage.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/Preferences/GeneralPage.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GeneralPage.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__GeneralPage_t {
    QByteArrayData data[8];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__GeneralPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__GeneralPage_t qt_meta_stringdata_Sn__GeneralPage = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Sn::GeneralPage"
QT_MOC_LITERAL(1, 16, 21), // "homePageActionChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 7), // "enabled"
QT_MOC_LITERAL(4, 47, 19), // "newTabActionChanged"
QT_MOC_LITERAL(5, 67, 20), // "startupActionChanged"
QT_MOC_LITERAL(6, 88, 12), // "currentIndex"
QT_MOC_LITERAL(7, 101, 18) // "saveCurrentSession"

    },
    "Sn::GeneralPage\0homePageActionChanged\0"
    "\0enabled\0newTabActionChanged\0"
    "startupActionChanged\0currentIndex\0"
    "saveCurrentSession"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__GeneralPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       4,    1,   37,    2, 0x08 /* Private */,
       5,    1,   40,    2, 0x08 /* Private */,
       7,    0,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,

       0        // eod
};

void Sn::GeneralPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GeneralPage *_t = static_cast<GeneralPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->homePageActionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->newTabActionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->startupActionChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->saveCurrentSession(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::GeneralPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__GeneralPage.data,
      qt_meta_data_Sn__GeneralPage,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::GeneralPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::GeneralPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__GeneralPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::GeneralPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
