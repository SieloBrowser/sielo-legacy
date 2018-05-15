/****************************************************************************
** Meta object code from reading C++ file 'AddressBar.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/AddressBar.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AddressBar.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__AddressDelegate_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__AddressDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__AddressDelegate_t qt_meta_stringdata_Sn__AddressDelegate = {
    {
QT_MOC_LITERAL(0, 0, 19) // "Sn::AddressDelegate"

    },
    "Sn::AddressDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__AddressDelegate[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Sn::AddressDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Sn::AddressDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_Sn__AddressDelegate.data,
      qt_meta_data_Sn__AddressDelegate,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::AddressDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::AddressDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__AddressDelegate.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int Sn::AddressDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_Sn__PopupViewWidget_t {
    QByteArrayData data[9];
    char stringdata0[124];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__PopupViewWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__PopupViewWidget_t qt_meta_stringdata_Sn__PopupViewWidget = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Sn::PopupViewWidget"
QT_MOC_LITERAL(1, 20, 16), // "canMoveUpChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 9), // "isAllowed"
QT_MOC_LITERAL(4, 48, 18), // "canMoveDownChanged"
QT_MOC_LITERAL(5, 67, 18), // "needsActionsUpdate"
QT_MOC_LITERAL(6, 86, 12), // "updateHeight"
QT_MOC_LITERAL(7, 99, 18), // "handleIndexEntered"
QT_MOC_LITERAL(8, 118, 5) // "index"

    },
    "Sn::PopupViewWidget\0canMoveUpChanged\0"
    "\0isAllowed\0canMoveDownChanged\0"
    "needsActionsUpdate\0updateHeight\0"
    "handleIndexEntered\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__PopupViewWidget[] = {

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
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       5,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   46,    2, 0x0a /* Public */,
       7,    1,   47,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    8,

       0        // eod
};

void Sn::PopupViewWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PopupViewWidget *_t = static_cast<PopupViewWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->canMoveUpChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->canMoveDownChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->needsActionsUpdate(); break;
        case 3: _t->updateHeight(); break;
        case 4: _t->handleIndexEntered((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (PopupViewWidget::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PopupViewWidget::canMoveUpChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (PopupViewWidget::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PopupViewWidget::canMoveDownChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (PopupViewWidget::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PopupViewWidget::needsActionsUpdate)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::PopupViewWidget::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_Sn__PopupViewWidget.data,
      qt_meta_data_Sn__PopupViewWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::PopupViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::PopupViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__PopupViewWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeView::qt_metacast(_clname);
}

int Sn::PopupViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
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
void Sn::PopupViewWidget::canMoveUpChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Sn::PopupViewWidget::canMoveDownChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Sn::PopupViewWidget::needsActionsUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_Sn__SideWidget_t {
    QByteArrayData data[3];
    char stringdata0[32];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__SideWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__SideWidget_t qt_meta_stringdata_Sn__SideWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "Sn::SideWidget"
QT_MOC_LITERAL(1, 15, 15), // "sizeHintChanged"
QT_MOC_LITERAL(2, 31, 0) // ""

    },
    "Sn::SideWidget\0sizeHintChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__SideWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void Sn::SideWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SideWidget *_t = static_cast<SideWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sizeHintChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SideWidget::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SideWidget::sizeHintChanged)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Sn::SideWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__SideWidget.data,
      qt_meta_data_Sn__SideWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::SideWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::SideWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__SideWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::SideWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Sn::SideWidget::sizeHintChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_Sn__AddressBar_t {
    QByteArrayData data[35];
    char stringdata0[391];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__AddressBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__AddressBar_t qt_meta_stringdata_Sn__AddressBar = {
    {
QT_MOC_LITERAL(0, 0, 14), // "Sn::AddressBar"
QT_MOC_LITERAL(1, 15, 13), // "setLeftMargin"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 6), // "margin"
QT_MOC_LITERAL(4, 37, 17), // "updateTextMargins"
QT_MOC_LITERAL(5, 55, 7), // "setText"
QT_MOC_LITERAL(6, 63, 4), // "text"
QT_MOC_LITERAL(7, 68, 7), // "showUrl"
QT_MOC_LITERAL(8, 76, 3), // "url"
QT_MOC_LITERAL(9, 80, 13), // "updateActions"
QT_MOC_LITERAL(10, 94, 18), // "updatePasteActions"
QT_MOC_LITERAL(11, 113, 7), // "sDelete"
QT_MOC_LITERAL(12, 121, 13), // "setCompletion"
QT_MOC_LITERAL(13, 135, 6), // "filter"
QT_MOC_LITERAL(14, 142, 11), // "sTextEdited"
QT_MOC_LITERAL(15, 154, 7), // "openUrl"
QT_MOC_LITERAL(16, 162, 5), // "index"
QT_MOC_LITERAL(17, 168, 14), // "requestLoadUrl"
QT_MOC_LITERAL(18, 183, 10), // "pasteAndGo"
QT_MOC_LITERAL(19, 194, 17), // "reloadStopClicked"
QT_MOC_LITERAL(20, 212, 18), // "setGoButtonVisible"
QT_MOC_LITERAL(21, 231, 5), // "state"
QT_MOC_LITERAL(22, 237, 11), // "loadStarted"
QT_MOC_LITERAL(23, 249, 12), // "loadProgress"
QT_MOC_LITERAL(24, 262, 8), // "progress"
QT_MOC_LITERAL(25, 271, 12), // "loadFinished"
QT_MOC_LITERAL(26, 284, 12), // "hideProgress"
QT_MOC_LITERAL(27, 297, 17), // "loadFromCompleter"
QT_MOC_LITERAL(28, 315, 8), // "QString&"
QT_MOC_LITERAL(29, 324, 12), // "loadSettings"
QT_MOC_LITERAL(30, 337, 9), // "fixedsize"
QT_MOC_LITERAL(31, 347, 10), // "leftMargin"
QT_MOC_LITERAL(32, 358, 10), // "fixedwidth"
QT_MOC_LITERAL(33, 369, 11), // "fixedheight"
QT_MOC_LITERAL(34, 381, 9) // "minHeight"

    },
    "Sn::AddressBar\0setLeftMargin\0\0margin\0"
    "updateTextMargins\0setText\0text\0showUrl\0"
    "url\0updateActions\0updatePasteActions\0"
    "sDelete\0setCompletion\0filter\0sTextEdited\0"
    "openUrl\0index\0requestLoadUrl\0pasteAndGo\0"
    "reloadStopClicked\0setGoButtonVisible\0"
    "state\0loadStarted\0loadProgress\0progress\0"
    "loadFinished\0hideProgress\0loadFromCompleter\0"
    "QString&\0loadSettings\0fixedsize\0"
    "leftMargin\0fixedwidth\0fixedheight\0"
    "minHeight"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__AddressBar[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       5,  152, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  114,    2, 0x0a /* Public */,
       4,    0,  117,    2, 0x0a /* Public */,
       5,    1,  118,    2, 0x0a /* Public */,
       7,    1,  121,    2, 0x0a /* Public */,
       9,    0,  124,    2, 0x08 /* Private */,
      10,    0,  125,    2, 0x08 /* Private */,
      11,    0,  126,    2, 0x08 /* Private */,
      12,    1,  127,    2, 0x08 /* Private */,
      14,    1,  130,    2, 0x08 /* Private */,
      15,    1,  133,    2, 0x08 /* Private */,
      17,    0,  136,    2, 0x08 /* Private */,
      18,    0,  137,    2, 0x08 /* Private */,
      19,    0,  138,    2, 0x08 /* Private */,
      20,    1,  139,    2, 0x08 /* Private */,
      22,    0,  142,    2, 0x08 /* Private */,
      23,    1,  143,    2, 0x08 /* Private */,
      25,    0,  146,    2, 0x08 /* Private */,
      26,    0,  147,    2, 0x08 /* Private */,
      27,    1,  148,    2, 0x08 /* Private */,
      29,    0,  151,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QUrl,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QModelIndex,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   21,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 28,    6,
    QMetaType::Void,

 // properties: name, type, flags
      30, QMetaType::QSize, 0x00095003,
      31, QMetaType::Int, 0x00095103,
      32, QMetaType::Int, 0x00095003,
      33, QMetaType::Int, 0x00095003,
      34, QMetaType::Int, 0x00095103,

       0        // eod
};

void Sn::AddressBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AddressBar *_t = static_cast<AddressBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setLeftMargin((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateTextMargins(); break;
        case 2: _t->setText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->showUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 4: _t->updateActions(); break;
        case 5: _t->updatePasteActions(); break;
        case 6: _t->sDelete(); break;
        case 7: _t->setCompletion((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->sTextEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->openUrl((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 10: _t->requestLoadUrl(); break;
        case 11: _t->pasteAndGo(); break;
        case 12: _t->reloadStopClicked(); break;
        case 13: _t->setGoButtonVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->loadStarted(); break;
        case 15: _t->loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->loadFinished(); break;
        case 17: _t->hideProgress(); break;
        case 18: _t->loadFromCompleter((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 19: _t->loadSettings(); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        AddressBar *_t = static_cast<AddressBar *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QSize*>(_v) = _t->size(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->leftMargin(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->width(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->height(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->minHeight(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        AddressBar *_t = static_cast<AddressBar *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFixedSize(*reinterpret_cast< QSize*>(_v)); break;
        case 1: _t->setLeftMargin(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setFixedWidth(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setFixedHeight(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setMinHeight(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Sn::AddressBar::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_Sn__AddressBar.data,
      qt_meta_data_Sn__AddressBar,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::AddressBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::AddressBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__AddressBar.stringdata0))
        return static_cast<void*>(this);
    return QLineEdit::qt_metacast(_clname);
}

int Sn::AddressBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
