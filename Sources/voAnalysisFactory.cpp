
// Qt includes
#include <QDebug>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisFactory.h"
#include "voQObjectFactory.h"

#include "voPCAStatistics.h"

//----------------------------------------------------------------------------
class voAnalysisFactoryPrivate
{
public:
  voQObjectFactory<voAnalysis> AnalysisFactory;
};

//----------------------------------------------------------------------------
// voAnalysisFactoryPrivate methods

//----------------------------------------------------------------------------
// voAnalysisFactory methods

//----------------------------------------------------------------------------
voAnalysisFactory::voAnalysisFactory():d_ptr(new voAnalysisFactoryPrivate)
{
  this->registerAnalysis<voPCAStatistics>("pca");
}

//-----------------------------------------------------------------------------
voAnalysisFactory::~voAnalysisFactory()
{
}

//-----------------------------------------------------------------------------
voAnalysis* voAnalysisFactory::createAnalysis(const QString& className)
{
  Q_D(voAnalysisFactory);
  return d->AnalysisFactory.Create(className);
}

//-----------------------------------------------------------------------------
QStringList voAnalysisFactory::registeredAnalysisNames() const
{
  Q_D(const voAnalysisFactory);
  return d->AnalysisFactory.registeredObjectKeys();
}

//-----------------------------------------------------------------------------
template<typename AnalysisClassType>
void voAnalysisFactory::registerAnalysis(const QString& analysisName)
{
  Q_D(voAnalysisFactory);
  
  if (analysisName.isEmpty())
    {
    qCritical() << "Failed to register analysis - analysisName is an empty string";
    return;
    }

  if (this->registeredAnalysisNames().contains(analysisName))
    {
    return;
    }

  d->AnalysisFactory.registerObject<AnalysisClassType>(analysisName);
}
