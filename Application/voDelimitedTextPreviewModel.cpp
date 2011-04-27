
//QT includes
#include <QDebug>
#include <QFile>
#include <QPalette>
#include <QStringList>
#include <QTemporaryFile>
#include <QTextStream>

//VTK includes
#include <vtkDelimitedTextReader.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

// Visomics includes
#include "voDelimitedTextPreviewModel.h"
#include "voUtils.h"


class voDelimitedTextPreviewModelPrivate
{
  Q_DECLARE_PUBLIC(voDelimitedTextPreviewModel);

public:
  voDelimitedTextPreviewModelPrivate(voDelimitedTextPreviewModel& object);

  void loadFile();

  void configureReader(vtkDelimitedTextReader * reader);

  QTemporaryFile SampleCacheFile;

  QString FileName;
  char FieldDelimiter;
  char StringDelimiter; // Value of 0 indicates none
  bool MergeConsecutiveDelimiters;
  bool Transpose;

  int ColumnMetaDataTypeOfInterest; // From 0..(N-1) with N = NumberOfColumnMetaDataTypes
  int NumberOfColumnMetaDataTypes;

  int RowMetaDataTypeOfInterest; // From 0..(N-1) with N = NumberOfRowMetaDataTypes
  int NumberOfRowMetaDataTypes;

  int NumberOfRowsToPreview;

  bool InlineUpdate;

private:
  voDelimitedTextPreviewModel* const q_ptr;
};

// --------------------------------------------------------------------------
// voDelimitedTextPreviewModelPrivate methods

// --------------------------------------------------------------------------
voDelimitedTextPreviewModelPrivate::voDelimitedTextPreviewModelPrivate(voDelimitedTextPreviewModel& object)
  : q_ptr(&object)
{
  this->FieldDelimiter = ',';
  this->StringDelimiter = '\"';
  this->MergeConsecutiveDelimiters = false;
  this->Transpose = false;

  this->ColumnMetaDataTypeOfInterest = 0;
  this->NumberOfColumnMetaDataTypes = 1;

  this->RowMetaDataTypeOfInterest = 0;
  this->NumberOfRowMetaDataTypes = 1;

  this->NumberOfRowsToPreview = 100;
  this->InlineUpdate = true;

  // If init() fails, SampleCacheFile will stay closed
  if (this->SampleCacheFile.isOpen())
    {
    this->SampleCacheFile.close();
    }
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModelPrivate::loadFile()
{
  Q_ASSERT(QFile::exists(this->FileName));
  QFile infile(this->FileName);
  bool openStatus = infile.open(QIODevice::ReadOnly);
  if (!openStatus)
    {
    qWarning() << QObject::tr("File ") << this->FileName << QObject::tr(" could not be opened for reading.  Did something change between when you selected the file and now?");
    return;
    }

  // Read file
  QTextStream instream(&infile);
  QStringList sampleLinesList;
  for (int i = 0; i < this->NumberOfRowsToPreview && !instream.atEnd(); i++)
    {
    sampleLinesList << instream.readLine();
    }

  // Push lines to temp file
  QString sampleLines;
  sampleLines = sampleLinesList.join("\n");
  bool status = this->SampleCacheFile.open();
  if (!status)
    {
    qWarning() << QObject::tr("Couldn't open temporary file for preview!");
    return;
    }
  this->SampleCacheFile.write(sampleLines.toAscii());

  this->SampleCacheFile.close();
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModelPrivate::configureReader(vtkDelimitedTextReader * reader)
{
  Q_ASSERT(reader);

  reader->DetectNumericColumnsOff();
  reader->SetFileName(this->SampleCacheFile.fileName().toAscii().data());

  char delim_string[2];
  delim_string[0] = this->FieldDelimiter;
  delim_string[1] = '\0';
  //vtkUnicodeString delimiters = vtkUnicodeString::from_utf8(delim_string);
  //reader->SetUnicodeFieldDelimiters(delimiters);
  reader->SetFieldDelimiterCharacters(delim_string);

  this->MergeConsecutiveDelimiters = (this->FieldDelimiter == ' ' || this->FieldDelimiter == '\t');
  reader->SetMergeConsecutiveDelimiters(this->MergeConsecutiveDelimiters);

  if (this->StringDelimiter) // Value of 0 indicates none
    {
    reader->SetStringDelimiter(this->StringDelimiter);
    reader->SetUseStringDelimiter(true);
    }
  else
    {
    reader->SetUseStringDelimiter(false);
    }

  reader->SetHaveHeaders(false);
}

// --------------------------------------------------------------------------
// voDelimitedTextPreviewModel methods

// --------------------------------------------------------------------------
voDelimitedTextPreviewModel::voDelimitedTextPreviewModel(QObject* newParent) :
  Superclass(newParent), d_ptr(new voDelimitedTextPreviewModelPrivate(*this))
{
  //Q_D(voDelimitedTextPreviewModel);
}

// --------------------------------------------------------------------------
voDelimitedTextPreviewModel::~voDelimitedTextPreviewModel()
{
}

// --------------------------------------------------------------------------
QString voDelimitedTextPreviewModel::fileName() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->FileName;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setFileName(const QString& newFileName)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->FileName == newFileName)
    {
    return;
    }

  if (!QFile::exists(newFileName))
    {
    return;
    }

  d->FileName = newFileName;

  d->loadFile();
  this->updatePreview();

  emit this->fileNameChanged(newFileName);
}

// --------------------------------------------------------------------------
QString voDelimitedTextPreviewModel::fieldDelimiterCharacters() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return QString(d->FieldDelimiter);
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setFieldDelimiter(char delimiter)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->FieldDelimiter == delimiter)
    {
    return;
    }
  d->FieldDelimiter = delimiter;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->fieldDelimiterChanged(delimiter);
}

// --------------------------------------------------------------------------
char voDelimitedTextPreviewModel::stringDelimiter() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->StringDelimiter;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setStringDelimiter(char character)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->StringDelimiter == character)
    {
    return;
    }
  d->StringDelimiter = character;
  if (d->InlineUpdate)
    {
    this->updatePreview();
    }
  emit this->stringDelimiterChanged(character);
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::transpose() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->Transpose;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setTranspose(bool value)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->Transpose == value)
    {
    return;
    }
  d->Transpose = value;

  // Switch row and column metdata type numbers
  int currentNumberOfColumnMetaDataTypes = d->NumberOfColumnMetaDataTypes;
  d->NumberOfColumnMetaDataTypes = d->NumberOfRowMetaDataTypes;
  d->NumberOfRowMetaDataTypes = currentNumberOfColumnMetaDataTypes;

  // Switch row and column metdata types of interest
  int currentColumnMetaDataTypeOfInterest = d->ColumnMetaDataTypeOfInterest;
  d->ColumnMetaDataTypeOfInterest = d->RowMetaDataTypeOfInterest;
  d->RowMetaDataTypeOfInterest = currentColumnMetaDataTypeOfInterest;

  // Do not call 'setNumberOfColumnMetaDataTypes()' to avoid extra call to 'updatePreview()'
  if (currentNumberOfColumnMetaDataTypes != d->NumberOfColumnMetaDataTypes)
    {
    // If column is changed, row is changed too (since they were swapped)
    emit this->numberOfColumnMetaDataTypesChanged(d->NumberOfColumnMetaDataTypes);
    emit this->numberOfRowMetaDataTypesChanged(d->NumberOfRowMetaDataTypes);
    }

  // Do not call 'setColumnMetaDataTypeOfInterest()' to avoid extra call to 'updatePreview()'
  if (currentColumnMetaDataTypeOfInterest != d->ColumnMetaDataTypeOfInterest)
    {
    emit this->columnMetaDataTypeOfInterestChanged(d->ColumnMetaDataTypeOfInterest);
    emit this->rowMetaDataTypeOfInterestChanged(d->RowMetaDataTypeOfInterest);
    }

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->transposeChanged(value);
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::numberOfColumnMetaDataTypes() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->NumberOfColumnMetaDataTypes;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setNumberOfColumnMetaDataTypes(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->NumberOfColumnMetaDataTypes == count)
    {
    return;
    }
  d->NumberOfColumnMetaDataTypes = count;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->numberOfColumnMetaDataTypesChanged(d->NumberOfColumnMetaDataTypes);

  // Constrain ColumnMetaDataTypeOfInterest
  if (count < this->columnMetaDataTypeOfInterest())
    {
    this->setColumnMetaDataTypeOfInterest(count);
    }
  if (count == 0)
    {
    this->setColumnMetaDataTypeOfInterest(-1);
    }
  else if (count > 0 && this->columnMetaDataTypeOfInterest() == -1)
    {
    this->setColumnMetaDataTypeOfInterest(0);
    }
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::columnMetaDataTypeOfInterest() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->ColumnMetaDataTypeOfInterest;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setColumnMetaDataTypeOfInterest(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->ColumnMetaDataTypeOfInterest == count)
    {
    return;
    }
  d->ColumnMetaDataTypeOfInterest = count;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->columnMetaDataTypeOfInterestChanged(d->ColumnMetaDataTypeOfInterest);
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::numberOfRowMetaDataTypes() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->NumberOfRowMetaDataTypes;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setNumberOfRowMetaDataTypes(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->NumberOfRowMetaDataTypes == count)
    {
    return;
    }
  d->NumberOfRowMetaDataTypes = count;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->numberOfRowMetaDataTypesChanged(d->NumberOfRowMetaDataTypes);

  // Constrain RowMetaDataTypeOfInterest
  if(count < this->rowMetaDataTypeOfInterest())
    {
    this->setRowMetaDataTypeOfInterest(count);
    }
  if (count == 0)
    {
    this->setRowMetaDataTypeOfInterest(-1);
    }
  else if (count > 0 && this->rowMetaDataTypeOfInterest() == -1)
    {
    this->setRowMetaDataTypeOfInterest(0);
    }
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::rowMetaDataTypeOfInterest() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->RowMetaDataTypeOfInterest;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setRowMetaDataTypeOfInterest(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->RowMetaDataTypeOfInterest == count)
    {
    return;
    }
  d->RowMetaDataTypeOfInterest = count;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->rowMetaDataTypeOfInterestChanged(d->RowMetaDataTypeOfInterest);
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::numberOfRowsToPreview() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->NumberOfRowsToPreview;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setNumberOfRowsToPreview(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->NumberOfRowsToPreview == count)
    {
    return;
    }
  d->NumberOfRowsToPreview = count;

  if (d->FileName.isEmpty())
    {
    return;
    }
  d->loadFile();
  this->updatePreview();

  emit this->numberOfRowsToPreviewChanged(count);
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::inlineUpdate() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->InlineUpdate;
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::mergeConsecutiveDelimiters()const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->MergeConsecutiveDelimiters;
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::useStringDelimiter()const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->StringDelimiter; // Value of 0 indicates None
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setInlineUpdate(bool value)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->InlineUpdate != value)
    {
    return;
    }
  d->InlineUpdate = value;
  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->inlineUpdateChanged(value);
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::updatePreview()
{
  Q_D(voDelimitedTextPreviewModel);

  if (d->SampleCacheFile.fileName().isEmpty())
    {
    //qWarning() << QObject::tr("ERROR: updatePreview: SampleCacheFile filename not set");
    return;
    }

  // Setup vtkDelimitedTextReader
  vtkNew<vtkDelimitedTextReader> previewReader;
  d->configureReader(previewReader.GetPointer());

  // Read in file to table
  previewReader->Update();
  vtkSmartPointer<vtkTable> table = previewReader->GetOutput();

  if (d->Transpose)
    {
    // Assumes there is a header column ... which we have no setting to specify for anyway
    voUtils::transposeTable(table);
    }

  // Build model (self)
  this->clear();

  QColor headerBackgroundColor = QPalette().color(QPalette::Window);
  QColor ofInterestBackgroundColor = QPalette().color(QPalette::Mid);

  for (vtkIdType cid = 0; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(table->GetColumn(cid));
    Q_ASSERT(column);
    for (int rid = 0; rid < column->GetNumberOfValues(); ++rid)
      {
      QString value = QString(column->GetValue(rid));

      QStandardItem* currentItem = 0;

      this->setItem(rid, cid, (currentItem = new QStandardItem(value)));
      currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

      if (rid == d->ColumnMetaDataTypeOfInterest || cid == d->RowMetaDataTypeOfInterest)
        {
        currentItem->setData(ofInterestBackgroundColor, Qt::BackgroundRole);
        }
      else if (rid < d->NumberOfColumnMetaDataTypes || cid < d->NumberOfRowMetaDataTypes)
        {
        currentItem->setData(headerBackgroundColor, Qt::BackgroundRole);
        }
      }
    }
}
