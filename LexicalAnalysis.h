#ifndef LEXICALANALYSIS_H
#define LEXICALANALYSIS_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QMap>
#include <QList>
#include <QTimer>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QTextEdit>
#include <QStringList>

// ===============
// NFA Structures
// ===============
struct NFAState {
    int id;
    bool isAccept = false;
};

struct NFATransition {
    int from;
    QString symbol; // "ε" or character
    int to;
};

struct NFA {
    int startState = -1;
    int acceptState = -1;
    QList<NFAState> states;
    QList<NFATransition> transitions;
    int nextId = 0;
    int getNextId() { return nextId++; }
};

// ===============
// Animation Step
// ===============
struct AnimationStep {
    QString fromState;
    QString toState;
    QString transitionKey;
};

// ===============
// DiagramElements
// ===============
struct DiagramElements {
    QMap<QString, QGraphicsEllipseItem*> states;
    QMap<QString, QList<QGraphicsItem*>> transitions;
};

// ===============
// DrawHelper
// ===============
class DrawHelper
{
public:
    static QGraphicsEllipseItem* createState(QGraphicsScene* scene, qreal x, qreal y, qreal size);
    static QGraphicsEllipseItem* createFinalState(QGraphicsScene* scene, qreal x, qreal y, qreal size);
    static QList<QGraphicsItem*> createArrow(QGraphicsScene* scene,
        QGraphicsEllipseItem* from,
        QGraphicsEllipseItem* to,
        const QString& label);
    static QList<QGraphicsItem*> createCurvedArrow(QGraphicsScene* scene,
        QGraphicsEllipseItem* from,
        QGraphicsEllipseItem* to,
        const QString& label,
        qreal curveOffset,
        qreal labelOffsetY);
    static QList<QGraphicsItem*> createSelfLoop(QGraphicsScene* scene,
        QGraphicsEllipseItem* state,
        const QString& label);
};

// ===============
// DiagramBuilder
// ===============
class DiagramBuilder
{
public:
    static DiagramElements buildDynamicDiagram(QGraphicsScene* scene, const QString& tokenType);
    
};

// ===============
// LexicalAnalysisTab
// ===============
class LexicalAnalysisTab : public QWidget
{
    Q_OBJECT

public:
    explicit LexicalAnalysisTab(QWidget *parent = nullptr);

signals:
    void tokensReady(const QList<QStringList>& tokens);
private slots:
    void runLexicalAnalysis();
    void animateNextStep();
    void onTokenClicked(QTableWidgetItem* item);

private:
    QTextEdit* userinput;
    QLabel* userlabel;
    QPushButton* run;
    QLabel* dfa;
    QGraphicsScene* dfaScene;
    QGraphicsView* dfaView;
    QLabel* tokenlabel;
    QTableWidget* tokenizationtable;
    QTimer* animationTimer;
    QList<QStringList> currentTokens;
    int currentTokenIndex;
    int currentStepIndex;
    QList<AnimationStep> currentSteps;
    DiagramElements diagramElements;

    QList<AnimationStep> getAnimationSteps(const QString& token, const QString& type);
    void resetHighlighting();
    void highlightState(QGraphicsEllipseItem* state);
    void highlightTransition(const QList<QGraphicsItem*>& items);
};

#endif // LEXICALANALYSIS_H
