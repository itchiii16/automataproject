#ifndef SYNTAXANALYSISTAB_H
#define SYNTAXANALYSISTAB_H

#include <QWidget>
#include <QList>

class QLabel;
class QTableWidget;
class QTextEdit;
class QPushButton;

class SyntaxAnalysisTab : public QWidget
{
    Q_OBJECT

public:
    SyntaxAnalysisTab(QWidget* parent = nullptr);
    void updateTokenTable(const QList<QList<QString>>& tokens);

private:
    // Token Table (Left Side)
    QLabel* tokenlabel;
    QTableWidget* tokenizationtable;

    // Unified PDA Parser (Right Side)
    QTextEdit* parserSimulator;
    QTextEdit* parserValidator;
    QPushButton* runParser;
};

#endif // SYNTAXANALYSISTAB_H