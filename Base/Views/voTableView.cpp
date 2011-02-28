
// Qt includes
#include <QDebug>
#include <QTableView>
#include <QLayout>
#include <QStandardItemModel>
#include <QWidget>

// Visomics includes
#include "voTableView.h"
#include "voDataObject.h"

// VTK includes
#include <vtkIdTypeArray.h>
#include <vtkQtTableRepresentation.h>
#include <vtkQtTableView.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voTableViewPrivate
{
public:
  voTableViewPrivate();

  QTableView*        TableView;
  QStandardItemModel Model;
};

// --------------------------------------------------------------------------
// voTableViewPrivate methods

// --------------------------------------------------------------------------
voTableViewPrivate::voTableViewPrivate()
{
  this->TableView = 0;
}

// --------------------------------------------------------------------------
// voTableView methods

// --------------------------------------------------------------------------
voTableView::voTableView(QWidget* newParent):
    Superclass(newParent), d_ptr(new voTableViewPrivate)
{
}

// --------------------------------------------------------------------------
voTableView::~voTableView()
{
}

// --------------------------------------------------------------------------
void voTableView::setupUi(QLayout *layout)
{
  Q_D(voTableView);

  d->TableView = new QTableView();
  d->TableView->setModel(&d->Model);

  layout->addWidget(d->TableView);
}

// --------------------------------------------------------------------------
void voTableView::setDataObject(voDataObject *dataObject)
{
  Q_D(voTableView);

  if (!dataObject)
    {
    qCritical() << "voTableView - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkTable * table = vtkTable::SafeDownCast(dataObject->data());
  if (!table)
    {
    qCritical() << "voTableView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  // Update the model if necessary
  //if (t->GetMTime() > this->LastModelMTime)
  //  {
    // Note: this will clear the current selection.
    vtkIdType num_rows = table->GetNumberOfRows();
    vtkIdType num_cols = table->GetNumberOfColumns();
    d->Model.setRowCount(static_cast<int>(num_cols - 1));
    d->Model.setColumnCount(static_cast<int>(num_rows));
    for (vtkIdType r = 0; r < num_rows; ++r)
      {
      d->Model.setHeaderData(static_cast<int>(r), Qt::Horizontal, QString(table->GetValue(r, 0).ToString()));
      for (vtkIdType c = 1; c < num_cols; ++c)
        {
        QStandardItem* item = new QStandardItem(QString(table->GetValue(r, c).ToString()));
        d->Model.setItem(static_cast<int>(c), static_cast<int>(r), item);
        }
      }
    for (vtkIdType c = 1; c < num_cols; ++c)
      {
      d->Model.setHeaderData(static_cast<int>(c), Qt::Vertical, QString(table->GetColumnName(c)));
      }
    d->TableView->hideRow(0);
  //  this->LastModelMTime = table->GetMTime();
  //  }

  // Retrieve the selected subtable
  //vtkSmartPointer<vtkTable> ot = vtkSmartPointer<vtkTable>::New();
  //this->selectedTable(ot);
  //this->Outputs["output"] = ot;
}

// --------------------------------------------------------------------------
//void voTableView::selectedTable(vtkTable* table)
//{
//  vtkTable* t = vtkTable::SafeDownCast(this->input().data());
//  if (!t)
//    {
//    return;
//    }

//  QSet<vtkIdType> rows;
//  QSet<vtkIdType> cols;

//  // Always select the first column (headers)
//  cols << 0;

//  const QModelIndexList selected = this->View->selectionModel()->selectedIndexes();
//  if (selected.size() == 0)
//    {
//    table->ShallowCopy(t);
//    return;
//    }
//  foreach (QModelIndex ind, selected)
//    {
//    cols << ind.row();
//    rows << ind.column();
//    }

//  foreach (vtkIdType c, cols)
//    {
//    vtkAbstractArray* col = t->GetColumn(c);
//    vtkAbstractArray* new_col = col->NewInstance();
//    new_col->SetName(col->GetName());
//    new_col->SetNumberOfTuples(rows.size());
//    table->AddColumn(new_col);
//    int ind = 0;
//    foreach (vtkIdType r, rows)
//      {
//      new_col->InsertTuple(ind, r, col);
//      ++ind;
//      }
//    new_col->Delete();
//    }
//}