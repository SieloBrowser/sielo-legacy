/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#include "AddressBarCompleterView.hpp"

#include <QApplication>

#include <QScrollBar>

#include <QEvent>
#include <QKeyEvent>

#include "Widgets/AddressBar/AddressBarCompleterDelegate.hpp"
#include "Widgets/AddressBar/AddressBarCompleterModel.hpp"

namespace Sn
{
AddressBarCompleterView::AddressBarCompleterView() :
	QListView(nullptr)
{
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAttribute(Qt::WA_X11NetWmWindowTypeCombo);
	setWindowFlags(Qt::Popup);


	setUniformItemSizes(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);

	setMouseTracking(true);

	QApplication::instance()->installEventFilter(this);

	m_delegate = new AddressBarCompleterDelegate(this);
	setItemDelegate(m_delegate);
}

void AddressBarCompleterView::setOriginalText(const QString& originalText)
{
	m_delegate->setOriginalText(originalText);
}

bool AddressBarCompleterView::eventFilter(QObject* object, QEvent* event)
{
	if (object == this || !isVisible())
		return false;

	switch (event->type()) {
	case QEvent::KeyPress: {
		QKeyEvent* keyEvent{static_cast<QKeyEvent*>(event)};
		Qt::KeyboardModifiers modifiers{keyEvent->modifiers()};
		const QModelIndex idx{currentIndex()};
		const QModelIndex visitSearchIdx{
			model()->index(0, 0).data(AddressBarCompleterModel::VisitSearchItemRole).toBool()
				? model()->index(0, 0)
				: QModelIndex()
		};

		if ((keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) && currentIndex() != idx)
			setCurrentIndex(idx);

		switch (keyEvent->key()) {
		case Qt::Key_Return:
		case Qt::Key_Enter:
			if (!idx.isValid())
				break;

			if (modifiers == Qt::NoModifier || modifiers == Qt::KeypadModifier) {
				emit indexActivated(idx);
				return true;
			}

			if (modifiers == Qt::ControlModifier) {
				emit indexCtrlActivated(idx);
				return true;
			}

			if (modifiers == Qt::ShiftModifier) {
				emit indexShiftActivated(idx);
				return true;
			}
			break;

		case Qt::Key_End:
			if (modifiers & Qt::ControlModifier) {
				setCurrentIndex(model()->index(model()->rowCount() - 1, 0));
				return true;
			}
			close();
			break;

		case Qt::Key_Home:
			if (modifiers & Qt::ControlModifier) {
				setCurrentIndex(model()->index(0, 0));
				scrollToTop();
				return true;
			}
			close();
			break;

		case Qt::Key_Escape:
			close();
			return true;

		case Qt::Key_F4:
			if (modifiers == Qt::AltModifier) {
				close();
				return false;
			}
			break;

		case Qt::Key_Tab:
		case Qt::Key_Backtab: {
			if (keyEvent->modifiers() != Qt::NoModifier)
				return false;

			Qt::Key k{keyEvent->key() == Qt::Key_Tab ? Qt::Key_Down : Qt::Key_Up};
			QKeyEvent ev{QKeyEvent::KeyPress, k, Qt::NoModifier };
			QApplication::sendEvent(focusProxy(), &ev);
			return true;
		}

		case Qt::Key_Up:
			if (!idx.isValid() || idx == visitSearchIdx) {
				int rowCount{model()->rowCount()};
				QModelIndex lastIndex{model()->index(rowCount - 1, 0)};

				setCurrentIndex(lastIndex);
			}
			else if (idx.row() == 0)
				setCurrentIndex(QModelIndex());
			else
				setCurrentIndex(model()->index(idx.row() - 1, 0));

			return true;

		case Qt::Key_Down:
			if (!idx.isValid()) {
				QModelIndex firstIndex{model()->index(0, 0)};
				setCurrentIndex(firstIndex);
			}
			else if (idx != visitSearchIdx && idx.row() == model()->rowCount() - 1) {
				setCurrentIndex(visitSearchIdx);
				scrollToTop();
			}
			else
				setCurrentIndex(model()->index(idx.row() + 1, 0));

			return true;

		case Qt::Key_Delete:
			if (idx != visitSearchIdx && viewport()->rect().contains(visualRect(idx))) {
				emit indexDeleteRequested(idx);
				return true;
			}
			break;

		case Qt::Key_PageUp:
			if (keyEvent->modifiers() != Qt::NoModifier)
				return false;

			selectionModel()->setCurrentIndex(moveCursor(QAbstractItemView::MovePageUp, Qt::NoModifier),
			                                  QItemSelectionModel::SelectCurrent);
			return true;

		case Qt::Key_PageDown:
			if (keyEvent->modifiers() != Qt::NoModifier)
				return false;

			selectionModel()->setCurrentIndex(moveCursor(QAbstractItemView::MovePageDown, Qt::NoModifier),
			                                  QItemSelectionModel::SelectCurrent);

			return true;

		case Qt::Key_Shift:
			if (idx != visitSearchIdx || underMouse()) {
				m_delegate->setShowSwitchToTab(false);
				viewport()->update();

				return true;
			}
			break;
		} // switch (keyEvent->key())

		if (focusProxy())
			(static_cast<QObject*>(focusProxy()))->event(keyEvent);

		return true;
	}

	case QEvent::KeyRelease: {
		QKeyEvent* keyEvent{static_cast<QKeyEvent*>(event)};

		switch (keyEvent->key()) {
		case Qt::Key_Shift:
			m_delegate->setShowSwitchToTab(true);
			viewport()->update();
			return true;
		}
		break;
	}

	case QEvent::Show:
		m_ignoreNextMouseMove = true;
		break;

	case QEvent::Wheel:
	case QEvent::MouseButtonPress:
		if (!underMouse()) {
			close();

			return false;
		}
		break;

	case QEvent::FocusOut: {
		QFocusEvent* focusEvent{static_cast<QFocusEvent*>(event)};
		if (focusEvent->reason() != Qt::PopupFocusReason && focusEvent->reason() != Qt::MouseFocusReason)
			close();

		break;
	}

	case QEvent::Move:
	case QEvent::Resize: {
		QWidget* w{qobject_cast<QWidget*>(object)};
		if (w && w->isWindow() && focusProxy() && w == focusProxy()->window())
			close();

		break;
	}

	default:
		break;
	} // switch (event->type())

	return false;
}

void AddressBarCompleterView::close()
{
	QListView::hide();
	verticalScrollBar()->setValue(0);

	m_delegate->setShowSwitchToTab(true);

	emit closed();
}

void AddressBarCompleterView::mouseReleaseEvent(QMouseEvent* event)
{
	QModelIndex idx{indexAt(event->pos())};

	if (!idx.isValid())
		return;

	Qt::MouseButton button{event->button()};
	Qt::KeyboardModifiers modifiers{event->modifiers()};

	if (button == Qt::LeftButton && modifiers == Qt::NoModifier) {
		emit indexActivated(idx);
		return;
	}

	if (button == Qt::MiddleButton || (button == Qt::LeftButton && modifiers == Qt::ControlModifier)) {
		emit indexCtrlActivated(idx);
		return;
	}

	if (button == Qt::LeftButton && modifiers == Qt::ShiftModifier) {
		emit indexShiftActivated(idx);
		return;
	}

	QListView::mouseReleaseEvent(event);
}
}
