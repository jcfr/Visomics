#ifndef __voApplication_h
#define __voApplication_h

// Qt includes
#include <QApplication>
#include <QScopedPointer>

class ctkErrorLogModel;
class QMainWindow;
class voAnalysisDriver;
class voAnalysisFactory;
class voAnalysisViewFactory;
class voApplicationPrivate;
class voDataModel;
class voIOManager;
class voRegistry;
class voViewFactory;
class voViewManager;

class voApplication : public QApplication
{
  Q_OBJECT
public:
  typedef QApplication Superclass;
  voApplication(int & argc, char ** argv);
  virtual ~voApplication();

  /// Return a reference to the application singleton
  static voApplication* application();
  
  /// Initialize application
  /// If exitWhenDone is True, it's your responsability to exit the application
  void initialize(bool& exitWhenDone);
  
  /// Return true if the application has been initialized
  /// \note initialize() should be called only one time.
  bool initialized() const;

  /// Return true is this instance of the application is running from an installed directory
  bool isInstalled()const;

  /// Get application home directory
  QString homeDirectory()const;

  QString rHome()const;

  voDataModel* dataModel()const;

  /// Get errorLogModel
  ctkErrorLogModel* errorLogModel()const;

  voAnalysisDriver* analysisDriver()const;

  voIOManager* ioManager()const;

  voViewManager* viewManager()const;

  voAnalysisFactory* analysisFactory()const;

  voRegistry* normalizerRegistry()const;

  voViewFactory* viewFactory()const;

  QMainWindow* mainWindow()const;
  
protected:
  QScopedPointer<voApplicationPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voApplication);
  Q_DISABLE_COPY(voApplication);
  
};

#endif
