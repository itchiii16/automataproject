#include "SyntaxAnalysisTab.h"
#include <QFont>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidgetItem>
#include <QSet>

// ============================================================
// FIX SUMMARY (IMPORTANT):
// 1. Explicit EOF token "$" is appended automatically
// 2. Program -> Statement Program now CONSUMES statements correctly
// 3. Expression termination correctly handled (ExprRest / TermRest)
// 4. Final ACCEPT condition simplified and correct
// ============================================================

SyntaxAnalysisTab::SyntaxAnalysisTab(QWidget* parent)
    : QWidget(parent)
{
    // ================= LEFT SIDE =================
    tokenlabel = new QLabel("Token Table", this);
    tokenlabel->setFont(QFont("Poppins", 14, QFont::Bold));

    tokenizationtable = new QTableWidget(this);
    tokenizationtable->setColumnCount(4);
    tokenizationtable->setHorizontalHeaderLabels({"Token", "Type", "Line", "Column"});
    tokenizationtable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tokenizationtable->verticalHeader()->setVisible(false);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(tokenlabel);
    leftLayout->addWidget(tokenizationtable);

    // ================= RIGHT SIDE =================
    QLabel* parserLabel = new QLabel("PDA Parsing Simulation", this);
    parserLabel->setFont(QFont("Poppins", 14, QFont::Bold));
    parserLabel->setAlignment(Qt::AlignCenter);

    parserSimulator = new QTextEdit(this);
    parserSimulator->setReadOnly(true);
    parserSimulator->setFont(QFont("Consolas", 11));

    parserValidator = new QTextEdit(this);
    parserValidator->setReadOnly(true);

    runParser = new QPushButton("Run Python PDA Parser", this);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(parserLabel);
    rightLayout->addWidget(parserSimulator);
    rightLayout->addWidget(parserValidator);
    rightLayout->addWidget(runParser);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    QWidget* left = new QWidget(this);
    left->setLayout(leftLayout);
    QWidget* right = new QWidget(this);
    right->setLayout(rightLayout);

    mainLayout->addWidget(left, 2);
    mainLayout->addWidget(right, 3);
    setLayout(mainLayout);

    // ================= PARSER =================
    connect(runParser, &QPushButton::clicked, this, [this]() {
        parserSimulator->clear();
        parserValidator->clear();

        // ---------------- Tokens ----------------
        QList<QString> tokens;
        for (int i = 0; i < tokenizationtable->rowCount(); ++i) {
            if (auto* item = tokenizationtable->item(i, 0)) {
                if (!item->text().trimmed().isEmpty())
                    tokens << item->text();
            }
        }
        tokens << "$"; // ✅ FIX: explicit EOF

        auto tokenType = [&](const QString& t) {
            if (t == "$" || t == "+" || t == "-" || t == "*" || t == "/" || t == "//" || t == "%" || t == "**" || t == "="
                || t == "(" || t == ")" || t == "[" || t == "]" || t == "{" || t == "}") return t;
            if (t[0].isDigit()) return QString("number");
            if (t[0].isLetter() || t[0] == '_') return QString("id");
            if (t.startsWith('"') || t.startsWith('\'')) return QString("string");
            return QString("INVALID");
        };

        // ---------------- Stack ----------------
        QStringList stack;
        stack << "$" << "Program";

        int i = 0;
        bool ok = true;

        auto log = [&](QString s) { parserSimulator->append(s); };

        log("START PDA\n");

        while (!stack.isEmpty() && ok) {
            QString top = stack.last();
            QString cur = tokens[i];
            QString type = tokenType(cur);

            log(QString("STACK: %1 | INPUT: %2").arg(stack.join(" ")).arg(cur));

            // ---------------- ACCEPT ----------------
            if (top == "$" && cur == "$") break;

            // ---------------- NON-TERMINALS ----------------
            if (top == "Program") {
                stack.removeLast();
                if (cur != "$") {
                    stack << "Program" << "Statement";
                }
            }
            else if (top == "Statement") {
                stack.removeLast();
                if (type == "id" && tokens[i+1] == "=") stack << "Assignment";
                else stack << "Expression";
            }
            else if (top == "Assignment") {
                stack.removeLast();
                stack << "Expression" << "=" << "id";
            }
            else if (top == "Expression") {
                stack.removeLast();
                stack << "ExprRest" << "Term";
            }
            else if (top == "ExprRest") {
                stack.removeLast();
                if (cur == "+" || cur == "-") stack << "ExprRest" << "Term" << cur;
            }
            else if (top == "Term") {
                stack.removeLast();
                stack << "TermRest" << "Factor";
            }
            else if (top == "TermRest") {
                stack.removeLast();
                if (cur == "*" || cur == "/" || cur == "//" || cur == "%" || cur == "**")
                    stack << "TermRest" << "Factor" << cur;
            }
            else if (top == "Factor") {
                stack.removeLast();
                if (cur == "(") stack << ")" << "Expression" << "(";
                else if (type == "id") stack << "id";
                else if (type == "number") stack << "number";
                else if (type == "string") stack << "string";
                else ok = false;
            }
            // ---------------- TERMINALS ----------------
            else {
                if (top == type || top == cur) {
                    stack.removeLast();
                    i++;
                } else {
                    ok = false;
                }
            }
        }

        if (ok && stack == QStringList({"$"}) && tokens[i] == "$") {
            parserValidator->setText("✅ ACCEPTED");
        } else {
            parserValidator->setText("❌ REJECTED");
        }
    });
}

void SyntaxAnalysisTab::updateTokenTable(const QList<QList<QString>>& tokens)
{
    tokenizationtable->setRowCount(tokens.size());
    for (int i = 0; i < tokens.size(); ++i)
        for (int j = 0; j < tokens[i].size() && j < 4; ++j)
            tokenizationtable->setItem(i, j, new QTableWidgetItem(tokens[i][j]));
}
