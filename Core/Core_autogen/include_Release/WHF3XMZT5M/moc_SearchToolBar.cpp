/****************************************************************************
** Meta object code from reading C++ file 'SearchToolBar.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/SearchToolBar.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SearchToolBar.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__FocusSelectLineEdit_t {
    QByteArrayData data[3];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__FocusSelectLineEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__FocusSelectLineEdit_t qt_meta_stringdata_Sn__FocusSelectLineEdit = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Sn::FocusSelectLineEdit"
QT_MOC_LITERAL(1, 24, 8), // "setFocus"
QT_MOC_LITERAL(2, 33, 0) // ""

    },
    "Sn::FocusSelectLineEdit\0setFocus\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__FocusSelectLineEdit[] = {

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
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Sn::FocusSelectLineEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FocusSelectLineEdit *_t = static_cast<FocusSelectLineEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setFocus(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Sn::FocusSelectLineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_Sn__FocusSelectLineEdit.data,
      qt_meta_data_Sn__FocusSelectLineEdit,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::FocusSelectLineEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::FocusSelectLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__FocusSelectLineEdit.stringdata0))
        return static_cast<void*>(this);
    return QLineEdit::qt_metacast(_clname);
}

int Sn::FocusSelectLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Sn__SearchToolBar_t {
    QByteArrayData data[9];
    char stringdata0[102];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__SearchToolBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__SearchToolBar_t qt_meta_stringdata_Sn__SearchToolBar = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Sn::SearchToolBar"
QT_MOC_LITERAL(1, 18, 10), // "searchText"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "text"
QT_MOC_LITERAL(4, 35, 15), // "updateFindFlags"
QT_MOC_LITERAL(5, 51, 22), // "caseSensitivityChanged"
QT_MOC_LITERAL(6, 74, 8), // "findNext"
QT_MOC_LITERAL(7, 83, 12), // "findPrevious"
QT_MOC_LITERAL(8, 96, 5) // "close"

    },
    "Sn::SearchToolBar\0searchText\0\0text\0"
    "updateFindFlags\0caseSensitivityChanged\0"
    "findNext\0findPrevious\0close"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__SearchToolBar[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x0a /* Public */,
       4,    0,   47,    2, 0x0a /* Public */,
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    0,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Sn::SearchToolBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SearchToolBar *_t = static_cast<SearchToolBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->searchText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->updateFindFlags(); break;
        case 2: _t->caseSensitivityChanged(); break;
        case 3: _t->findNext(); break;
        case 4: _t->findPrevious(); break;
        case 5: _t->close(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::SearchToolBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__SearchToolBar.data,
      qt_meta_data_Sn__SearchToolBar,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::SearchToolBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::SearchToolBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__SearchToolBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::SearchToolBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
