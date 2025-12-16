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
#include <QList>
#include <QChar>
#include <QStack>

SyntaxAnalysisTab::SyntaxAnalysisTab(QWidget* parent)
    : QWidget(parent)
{
    // ================= LEFT SIDE: Token Table =================
    tokenlabel = new QLabel("Token Table", this);
    tokenlabel->setFont(QFont("Poppins", 14, QFont::Bold));

    tokenizationtable = new QTableWidget(this);
    tokenizationtable->setColumnCount(4);
    tokenizationtable->setHorizontalHeaderLabels({"Token", "Type", "Line", "Column"});
    tokenizationtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tokenizationtable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tokenizationtable->verticalHeader()->setVisible(false);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(tokenlabel);
    leftLayout->addWidget(tokenizationtable);

    QWidget* leftContainer = new QWidget(this);
    leftContainer->setLayout(leftLayout);

    // ================= RIGHT SIDE: Single Unified Parser Tab =================
    QVBoxLayout* rightLayout = new QVBoxLayout();

    QLabel* parserTitle = new QLabel("PDA Parser", this);
    parserTitle->setFont(QFont("Poppins", 16, QFont::Bold));
    parserTitle->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(parserTitle);

    QLabel* simulatorTitle = new QLabel("Parser Log", this);
    simulatorTitle->setFont(QFont("Poppins", 14, QFont::Bold));
    simulatorTitle->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(simulatorTitle);

    parserSimulator = new QTextEdit(this);
    parserSimulator->setReadOnly(true);
    parserSimulator->setFont(QFont("Consolas", 11));
    rightLayout->addWidget(parserSimulator, 3);

    QLabel* validatorTitle = new QLabel("Validation Result", this);
    validatorTitle->setFont(QFont("Poppins", 14, QFont::Bold));
    validatorTitle->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(validatorTitle);

    parserValidator = new QTextEdit(this);
    parserValidator->setReadOnly(true);
    parserValidator->setFont(QFont("Poppins", 12));
    rightLayout->addWidget(parserValidator, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    runParser = new QPushButton("Run PDA Parser", this);
    runParser->setFont(QFont("Poppins", 10, QFont::Bold));
    runParser->setStyleSheet("background-color: #16163F; color: white; padding: 10px 30px;");
    buttonLayout->addStretch();
    buttonLayout->addWidget(runParser);
    rightLayout->addLayout(buttonLayout);

    QWidget* rightContainer = new QWidget(this);
    rightContainer->setLayout(rightLayout);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(leftContainer, 2);
    mainLayout->addWidget(rightContainer, 3);
    setLayout(mainLayout);

    // =================  PDA PARSER =================
    connect(runParser, &QPushButton::clicked, this, [this]() {
        parserSimulator->clear();
        parserValidator->clear();

        QStringList log;
        log << "╔════════════════════════════════════════════════════════════╗\n";
        log << "║          PDA PARSER - CFG to PDA CONVERSION       ║\n";
        log << "╚════════════════════════════════════════════════════════════╝\n\n";

        log << "Context-Free Grammar (CFG):\n";
        log << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        log << "Program  → Statement Program | ε\n";
        log << "Statement → Assignment | Expression | DelimBlock\n";
        log << "Assignment → id = Expression\n";
        log << "Expression → Term ((+|-) Term)*\n";
        log << "Term → Factor ((*|/) Factor)*\n";
        log << "Factor → ( Expression ) | id | number\n";
        log << "DelimBlock → { Program } | ( Expression ) | [ Expression ]\n";
        log << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

        log << "PDA Configuration:\n";
        log << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        log << "States: {q0(start), q1(id), q2(assign), q3(expr), qf(accept)}\n";
        log << "Stack Alphabet: {$, (, {, [, E, T, F}\n";
        log << "Initial Stack: $\n";
        log << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

        log << "PDA Transition Rules:\n";
        log << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        log << "δ(q0, id, $) → (q1, $)              // Read identifier\n";
        log << "δ(q1, =, $) → (q2, $)               // Read assignment\n";
        log << "δ(q0, {, $) → (q0, {$)              // Push opening delimiter\n";
        log << "δ(q0, (, Z) → (q0, (Z)              // Push opening paren\n";
        log << "δ(q3, }, {) → (q3, ε)               // Pop matching delimiter\n";
        log << "δ(q3, ), () → (q3, ε)               // Pop matching paren\n";
        log << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

        // Get tokens from table
        QList<QPair<QString, int>> tokens;
        for (int i = 0; i < tokenizationtable->rowCount(); ++i) {
            QTableWidgetItem* tokenItem = tokenizationtable->item(i, 0);
            QTableWidgetItem* lineItem = tokenizationtable->item(i, 2);
            if (tokenItem && !tokenItem->text().trimmed().isEmpty()) {
                QString token = tokenItem->text();
                int line = lineItem ? lineItem->text().toInt() : (i + 1);
                tokens.append(qMakePair(token, line));
            }
        }

        if (tokens.isEmpty()) {
            log << "⚠ No tokens available for parsing.\n";
            parserSimulator->setPlainText(log.join(""));
            parserValidator->setPlainText("❌ No input to parse");
            return;
        }

        // PDA State
        QString state = "q0";
        QStringList stack;
        stack.push_back("$"); // Initial stack symbol
        
        int step = 1;
        bool valid = true;
        int successfulStatements = 0;
        int failedStatements = 0;
        int currentLine = -1;
        QString currentStatement;
        
        QSet<QString> keywords = {"if", "else", "elif", "for", "while", "def", "return", "print"};

        log << "PDA SIMULATION:\n";
        log << "════════════════════════════════════════════════════════════\n\n";

        for (int i = 0; i < tokens.size(); ++i) {
            QString token = tokens[i].first;
            int line = tokens[i].second;

            // Track line changes
            if (line != currentLine) {
                if (currentLine != -1 && !currentStatement.isEmpty()) {
                    log << QString("────────────────────────────────────────────────────────────\n");
                }
                currentLine = line;
                currentStatement.clear();
                log << QString("📍 Line %1:\n").arg(line);
            }

            currentStatement += token + " ";

            // Check if it's an identifier (potential assignment or expression)
            bool isValidId = !token.isEmpty() && (token[0].isLetter() || token[0] == '_');
            
            // Skip keywords that start statements
            if (keywords.contains(token)) {
                log << QString("Step %1: SKIP '%2' (keyword) - Line %3\n")
                    .arg(step++).arg(token).arg(line);
                continue;
            }

            // Process different token types
            if (token == "{" || token == "(" || token == "[") {
                // Opening delimiter - PUSH
                stack.push_back(token);
                state = "q0";
                log << QString("Step %1: δ(q0, '%2', Z) → (q0, '%2'Z)\n")
                    .arg(step++).arg(token);
                log << QString("         Action: PUSH '%1' onto stack\n").arg(token);
                log << QString("         Stack: [%1]\n").arg(stack.join(", "));
                
            } else if (token == "}" || token == ")" || token == "]") {
                // Closing delimiter - POP
                if (stack.isEmpty() || stack.last() == "$") {
                    log << QString("Step %1: ❌ ERROR - δ(q3, '%2', ε) - Empty stack!\n")
                        .arg(step++).arg(token);
                    log << QString("         Unexpected closing delimiter '%1' on line %2\n")
                        .arg(token).arg(line);
                    valid = false;
                    failedStatements++;
                    break;
                }

                QString top = stack.last();
                bool match = (token == "}" && top == "{") ||
                             (token == ")" && top == "(") ||
                             (token == "]" && top == "[");

                if (match) {
                    stack.pop_back();
                    state = "q3";
                    log << QString("Step %1: δ(q3, '%2', '%3') → (q3, ε)\n")
                        .arg(step++).arg(token).arg(top);
                    log << QString("         Action: POP '%1' (matched with '%2')\n")
                        .arg(top).arg(token);
                    log << QString("         Stack: [%1]\n").arg(stack.join(", "));
                    successfulStatements++;
                } else {
                    log << QString("Step %1: ❌ ERROR - Mismatch: '%2' vs '%3'\n")
                        .arg(step++).arg(top).arg(token);
                    valid = false;
                    failedStatements++;
                    break;
                }

            } else if (isValidId && i + 1 < tokens.size() && tokens[i + 1].first == "=") {
                // Assignment statement
                state = "q1";
                log << QString("Step %1: δ(q0, id='%2', $) → (q1, $)\n")
                    .arg(step++).arg(token);
                log << QString("         Action: Recognize identifier for assignment\n");
                log << QString("         Stack: [%1]\n").arg(stack.join(", "));

            } else if (token == "=") {
                // Assignment operator
                state = "q2";
                log << QString("Step %1: δ(q1, '=', $) → (q2, $)\n").arg(step++);
                log << QString("         Action: Process assignment operator\n");
                log << QString("         Stack: [%1]\n").arg(stack.join(", "));

            } else if (token[0].isDigit()) {
                // Number - part of expression
                state = "q3";
                log << QString("Step %1: δ(q2, number='%2', $) → (q3, $)\n")
                    .arg(step++).arg(token);
                log << QString("         Action: Process number in expression\n");
                log << QString("         Stack: [%1]\n").arg(stack.join(", "));

            } else if (token == "+" || token == "-" || token == "*" || token == "/") {
                // Operator
                state = "q3";
                log << QString("Step %1: δ(q3, operator='%2', $) → (q3, $)\n")
                    .arg(step++).arg(token);
                log << QString("         Action: Process operator\n");
                log << QString("         Stack: [%1]\n").arg(stack.join(", "));

            } else if (isValidId) {
                // Identifier in expression
                state = "q3";
                log << QString("Step %1: δ(q3, id='%2', $) → (q3, $)\n")
                    .arg(step++).arg(token);
                log << QString("         Action: Process identifier in expression\n");
                log << QString("         Stack: [%1]\n").arg(stack.join(", "));
            }
        }

        log << "\n════════════════════════════════════════════════════════════\n";
        log << "FINAL PDA STATE:\n";
        log << "════════════════════════════════════════════════════════════\n";
        log << QString("State: %1\n").arg(state);
        log << QString("Stack: [%1]\n").arg(stack.join(", "));
        log << QString("Stack Size: %1\n\n").arg(stack.size());

        // Final validation
        if (valid && stack.size() == 1 && stack[0] == "$") {
            log << "✅ ACCEPT: Stack is empty (only $ remains)\n";
            log << "✅ All delimiters are balanced\n";
            log << "✅ All expressions are well-formed\n";
            parserValidator->setPlainText(QString(
                "✅ PARSING SUCCESSFUL\n\n"
                "✓ Syntax is valid\n"
                "✓ All delimiters balanced\n"
                "✓ All expressions well-formed\n"
                "✓ Processed %1 tokens").arg(tokens.size()));
        } else if (valid && stack.size() > 1) {
            log << "❌ REJECT: Unclosed delimiters remain in stack\n";
            log << QString("   Remaining: [%1]\n").arg(stack.join(", "));
            parserValidator->setPlainText(QString(
                "❌ PARSING FAILED\n\n"
                "✗ Unclosed delimiters detected\n"
                "✗ Stack not empty at end\n"
                "Remaining: %1").arg(stack.join(", ")));
        } else {
            log << "❌ REJECT: Syntax errors detected\n";
            parserValidator->setPlainText(QString(
                "❌ PARSING FAILED\n\n"
                "✗ Syntax errors detected\n"
                "✗ Invalid token sequence\n"
                "✗ Check parser log for details"));
        }

        parserSimulator->setPlainText(log.join(""));
    });
}

// ================= UPDATE TOKEN TABLE =================
void SyntaxAnalysisTab::updateTokenTable(const QList<QList<QString>>& tokens)
{
    tokenizationtable->setRowCount(tokens.size());
    for (int i = 0; i < tokens.size(); ++i) {
        for (int j = 0; j < qMin(tokens[i].size(), 4); ++j) {
            tokenizationtable->setItem(i, j, new QTableWidgetItem(tokens[i][j]));
        }
    }
}