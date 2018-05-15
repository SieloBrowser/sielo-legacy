/****************************************************************************
** Meta object code from reading C++ file 'MasterPasswordDialog.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Password/MasterPasswordDialog.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MasterPasswordDialog.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__MasterPasswordDialog_t {
    QByteArrayData data[12];
    char stringdata0[165];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__MasterPasswordDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__MasterPasswordDialog_t qt_meta_stringdata_Sn__MasterPasswordDialog = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Sn::MasterPasswordDialog"
QT_MOC_LITERAL(1, 25, 6), // "accept"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 6), // "reject"
QT_MOC_LITERAL(4, 40, 16), // "showSettingsPage"
QT_MOC_LITERAL(5, 57, 17), // "showSetMasterPage"
QT_MOC_LITERAL(6, 75, 29), // "clearMasterPasswordAddConvert"
QT_MOC_LITERAL(7, 105, 13), // "forcedAskPass"
QT_MOC_LITERAL(8, 119, 17), // "samePasswordEntry"
QT_MOC_LITERAL(9, 137, 13), // "PasswordEntry"
QT_MOC_LITERAL(10, 151, 6), // "entry1"
QT_MOC_LITERAL(11, 158, 6) // "entry2"

    },
    "Sn::MasterPasswordDialog\0accept\0\0"
    "reject\0showSettingsPage\0showSetMasterPage\0"
    "clearMasterPasswordAddConvert\0"
    "forcedAskPass\0samePasswordEntry\0"
    "PasswordEntry\0entry1\0entry2"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__MasterPasswordDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    0,   51,    2, 0x0a /* Public */,
       5,    0,   52,    2, 0x0a /* Public */,
       6,    1,   53,    2, 0x0a /* Public */,
       6,    0,   56,    2, 0x2a /* Public | MethodCloned */,
       8,    2,   57,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Bool, 0x80000000 | 9, 0x80000000 | 9,   10,   11,

       0        // eod
};

void Sn::MasterPasswordDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MasterPasswordDialog *_t = static_cast<MasterPasswordDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->accept(); break;
        case 1: _t->reject(); break;
        case 2: _t->showSettingsPage(); break;
        case 3: _t->showSetMasterPage(); break;
        case 4: _t->clearMasterPasswordAddConvert((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->clearMasterPasswordAddConvert(); break;
        case 6: { bool _r = _t->samePasswordEntry((*reinterpret_cast< const PasswordEntry(*)>(_a[1])),(*reinterpret_cast< const PasswordEntry(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::MasterPasswordDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Sn__MasterPasswordDialog.data,
      qt_meta_data_Sn__MasterPasswordDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::MasterPasswordDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::MasterPasswordDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__MasterPasswordDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Sn::MasterPasswordDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
struct qt_meta_stringdata_Sn__AskMasterPasswordDialog_t {
    QByteArrayData data[3];
    char stringdata0[44];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__AskMasterPasswordDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__AskMasterPasswordDialog_t qt_meta_stringdata_Sn__AskMasterPasswordDialog = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Sn::AskMasterPasswordDialog"
QT_MOC_LITERAL(1, 28, 14), // "verifyPassword"
QT_MOC_LITERAL(2, 43, 0) // ""

    },
    "Sn::AskMasterPasswordDialog\0verifyPassword\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__AskMasterPasswordDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Sn::AskMasterPasswordDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AskMasterPasswordDialog *_t = static_cast<AskMasterPasswordDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->verifyPassword(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Sn::AskMasterPasswordDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Sn__AskMasterPasswordDialog.data,
      qt_meta_data_Sn__AskMasterPasswordDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::AskMasterPasswordDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::AskMasterPasswordDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__AskMasterPasswordDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Sn::AskMasterPasswordDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
