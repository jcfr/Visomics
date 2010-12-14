

// Qt includes
#include <QScopedPointer>
#include <QMainWindow>
#include <QMap>

class QDockWidget;
class QSplitter;
class QTreeWidget;
class QTableWidget;

class voAnalysisPanel;
class voCSVReader;
class voAnalysis;

class voMainWindowPrivate;

class voMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  typedef QMainWindow Superclass;
  voMainWindow(QWidget * newParent = 0);
  virtual ~voMainWindow();
  
public slots:
  void onFileOpenActionTriggered();

  void about();

protected:
  QScopedPointer<voMainWindowPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voMainWindow);
  Q_DISABLE_COPY(voMainWindow);
};
