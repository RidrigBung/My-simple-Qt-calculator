#include "calculator.h"
#include "mybutton.h"
#include <QGridLayout>
#include <QToolButton>
#include <QDebug>
#include <QtMath>

Calculator::Calculator()
{
    m_display_up   = new QLineEdit();
    m_display_down = new QLineEdit();
    m_sign         = new QLabel();
    QGridLayout *mainLayout = new QGridLayout;

    m_display_down->setText("0");
    m_sum_in_memory = 0;

    m_sign->setAlignment(Qt::AlignRight);

    m_display_up->setReadOnly(true);
    m_display_down->setReadOnly(true);

    m_display_up->setAlignment(Qt::AlignRight);
    m_display_down->setAlignment(Qt::AlignRight);

    m_display_up->setMaxLength(16);
    m_display_down->setMaxLength(16);

    QFont font = m_display_up->font();
    font.setPointSize(font.pointSize() + 8);
    m_display_up->setFont(font);
    m_display_down->setFont(font);
    m_sign->setFont(font);

    // Создаем все конпки на калькуляторе
    for (int i = 0; i < 10; i++) {
        m_digit_buttons[i] = createButton(QString::number(i), SLOT(digitClicked()));
    }

    MyButton *pointButton = createButton(".", SLOT(pointClicked()));
    MyButton *changeSignButton = createButton(m_change_sign, SLOT(changeSignClicked()));

    MyButton *backspaceButton = createButton("Backspace", SLOT(backspaceClicked()));
    MyButton *clearButton = createButton("Clear", SLOT(clear()));
    MyButton *clearAllButton = createButton("Clear All", SLOT(clearAll()));

    MyButton *clearMemoryButton = createButton("MC", SLOT(clearMemory()));
    MyButton *readMemoryButton = createButton("MR", SLOT(readMemory()));
    MyButton *addToMemoryButton = createButton("M+", SLOT(addToMemory()));
    MyButton *minToMemoryButton = createButton("M-", SLOT(minToMemory()));

    MyButton *divisionButton = createButton(m_division_sign, SLOT(doubleOperandClicked()));
    MyButton *timesButton = createButton(m_times_sign, SLOT(doubleOperandClicked()));
    MyButton *minusButton = createButton(m_minus_sign, SLOT(doubleOperandClicked()));
    MyButton *plusButton = createButton(m_plus_sign, SLOT(doubleOperandClicked()));

    MyButton *squareRootButton = createButton(m_squareRoot_sign, SLOT(unaryOperatorClicked()));
    MyButton *powerButton = createButton(m_power_sign, SLOT(unaryOperatorClicked()));
    MyButton *reciprocalButton = createButton(m_reciprocal_sign, SLOT(unaryOperatorClicked()));

    MyButton *equalButton = createButton("=", SLOT(equalClicked()));

    // Размещаем виджеты на дислее
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(m_display_up,    0, 0, 1, 6);
    mainLayout->addWidget(m_sign,          1, 5, 1, 1);
    mainLayout->addWidget(m_display_down,  2, 0, 1, 6);
    mainLayout->addWidget(backspaceButton, 3, 0, 1, 2);
    mainLayout->addWidget(clearButton,     3, 2, 1, 2);
    mainLayout->addWidget(clearAllButton,  3, 4, 1, 2);

    mainLayout->addWidget(clearMemoryButton, 4, 0);
    mainLayout->addWidget(readMemoryButton,  5, 0);
    mainLayout->addWidget(minToMemoryButton, 6, 0);
    mainLayout->addWidget(addToMemoryButton, 7, 0);

    for (int i = 1; i < 10; ++i) {
        int row = ((9 - i) / 3) + 4;
        int column = ((i - 1) % 3) + 1;
        mainLayout->addWidget(m_digit_buttons[i], row, column);
    }

    mainLayout->addWidget(m_digit_buttons[0], 7, 1);
    mainLayout->addWidget(pointButton,       7, 2);
    mainLayout->addWidget(changeSignButton,  7, 3);

    mainLayout->addWidget(divisionButton, 4, 4);
    mainLayout->addWidget(timesButton,    5, 4);
    mainLayout->addWidget(minusButton,    6, 4);
    mainLayout->addWidget(plusButton,     7, 4);

    mainLayout->addWidget(squareRootButton, 4, 5);
    mainLayout->addWidget(powerButton,      5, 5);
    mainLayout->addWidget(reciprocalButton, 6, 5);
    mainLayout->addWidget(equalButton,      7, 5);

    setLayout(mainLayout);
    setWindowTitle("Calculator");
}

MyButton* Calculator::createButton(const QString &text, const char *member)
{
    MyButton* btn = new MyButton(text);
    connect(btn, SIGNAL(clicked()), this, member);
    return btn;
}

void Calculator::digitClicked() {
    MyButton* btn = (MyButton*) sender();
    int digit = btn->text().toUInt();
    qDebug() << "Digit clicked!" << digit;

    if (m_display_down->text() == "0" || m_display_down->text() == "Math error!") {
        m_display_down->clear();
    }
    m_display_down->setText(m_display_down->text() + QString::number(digit));
}

void Calculator::unaryOperatorClicked() {
    qDebug() << "Unary operator clicked!";
    MyButton* btn = (MyButton*) sender();
    QString operation = btn->text();

    double operand = m_display_down->text().toDouble();
    double result = 0.0;

    if (operation == m_squareRoot_sign) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = std::sqrt(operand);
    }

    else if (operation == m_power_sign) {
        result = std::pow(operand, 2.0);
    }

    else if (operation == m_reciprocal_sign) {
        if (operand == 0.0) {
            abortOperation();
            return;
        }
        result = 1.0 / operand;
    }

    m_display_down->setText(QString::number(result));
}

void Calculator::doubleOperandClicked() {
    qDebug() << "Double operator clicked!";
    MyButton* btn = (MyButton*) sender();
    QString operation = btn->text();

    double operand = m_display_down->text().toDouble();

    //if (m_display_down->text() == "0" && m_display_up->text() == "") {
    //    return;
    //}

    m_sign->setText(operation);

    if (m_display_down->text() == "") {
        m_pending_operation = operation;
        return;
    }

    m_display_down->clear();

    if (!m_pending_operation.isEmpty()) {
        if (!calculate(operand, m_pending_operation)) {
            abortOperation();
            return;
        }
        m_pending_operation.clear();
    }
    else {
        m_display_up->setText(QString::number(operand));
    }

    m_pending_operation = operation;
}

void Calculator::equalClicked() {
    double operand = m_display_down->text().toDouble();

    if (!m_pending_operation.isEmpty()) {
        if (!calculate(operand, m_pending_operation)) {
            abortOperation();
            return;
        }
        m_pending_operation.clear();
        m_display_down->setText(m_display_up->text());
        m_display_up->clear();
        m_sign->clear();
    }
}

void Calculator::pointClicked() {
    if (!m_display_down->text().contains('.')) {
        if (m_display_down->text() == "") {
            m_display_down->setText("0.");
        }
        else {
            m_display_down->setText(m_display_down->text() + '.');
        }
    }
}

void Calculator::changeSignClicked() {
    qDebug() << "Change Sign Clicked";
    QString txt = m_display_down->text();
    double val = txt.toDouble();

    if (val > 0) {
        txt.prepend("-");
    }
    else if (val < 0) {
        txt.remove(0, 1);
    }

    m_display_down->setText(txt);
}

void Calculator::backspaceClicked() {
    QString txt = m_display_down->text();
    txt.chop(1);
    if (txt.isEmpty()) {
        txt = "0";
    }
    m_display_down->setText(txt);
}

// Чистит только 1 строку
/* Этот вариант функции удаляет и оператор если он введен
void Calculator::clear() {
    if (m_display_up->text() != "") {
        m_pending_operation.clear();
        m_sign->clear();
        m_display_down->setText(m_display_up->text());
        m_display_up->clear();
    }
    else {
        m_display_down->setText("0");
    }
}
*/

// Если нижняя строка пуста а верхняя нет, значит применён операнд.
// При нажатии clear в таком случае верхняя строка переместится вниз а операнд будет удалён.
void Calculator::clear() {
    if (m_display_down->text() == "0" && m_display_up->text() != "") {
        QString up_text = m_display_up->text();
        clearAll();
        m_display_down->setText(up_text);
    }
    else {
        m_display_down->setText("0");
    }
}

// Чистит обе строки и оператор
void Calculator::clearAll() {
    m_display_up->clear();
    m_pending_operation.clear();
    m_sign->clear();
    m_display_down->setText("0");
}

void Calculator::clearMemory() {
    m_sum_in_memory = 0.0;
}

void Calculator::readMemory() {
    //m_display_up->clear();
    //m_sign->clear();
    //m_pending_operation.clear();
    m_display_down->setText(QString::number(m_sum_in_memory));
}

void Calculator::addToMemory() {
    equalClicked();
    m_sum_in_memory += m_display_down->text().toDouble();
}

void Calculator::minToMemory() {
    equalClicked();
    m_sum_in_memory -= m_display_down->text().toDouble();
}

void Calculator::abortOperation() {
    clear();
    m_display_down->setText("Math error!");
}

bool Calculator::calculate(double operand, const QString &operation) {
    double temp_total = m_display_up->text().toDouble();

    if (operation == m_plus_sign) {
        temp_total += operand;
    }
    else if (operation == m_minus_sign) {
        temp_total -= operand;
    }
    else if (operation == m_times_sign) {
        temp_total *= operand;
    }
    else if (operation == m_division_sign) {
        if (operand == 0.0) {
            abortOperation();
            return false;
        }
        temp_total /= operand;
    }
    m_display_up->setText(QString::number(temp_total));
    return true;
}
