#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>
#include <QDateTime>
#include <QRegularExpression>
#include <QSet>
#include <cmath>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
}

MainWindow::~MainWindow() {}

void MainWindow::on_btnCheck_clicked() { analyzeText(); }

void MainWindow::analyzeText() {
    QString text = ui->textEdit->toPlainText();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст для анализа");
        return;
    }

    bool ignoreCase = ui->chkIgnoreCase->isChecked();

    int totalChars = text.length();
    int charsNoSpaces = text.count(QRegularExpression("\\S"));

    QString processedText = text;
    if (ignoreCase) {
        processedText = processedText.toLower();
    }

    QStringList words = processedText.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    int wordCount = words.size();

    double uniqueness = 100.0;
    if (wordCount > 0) {
        QSet<QString> plagiarismWords = getPlagiarismWords();
        int plagiarismWordCount = 0;
        for (const QString& word : words) {
            if (plagiarismWords.contains(word)) {
                plagiarismWordCount++;
            }
        }
        uniqueness = (1.0 - static_cast<double>(plagiarismWordCount) / wordCount) * 100;
    }

    updateAnalysisResults(uniqueness, totalChars, charsNoSpaces, wordCount);
}

QSet<QString> MainWindow::getPlagiarismWords() const {
    QSet<QString> plagiarismWords;
    plagiarismWords.insert("кот");
    plagiarismWords.insert("собака");
    plagiarismWords.insert("дом");
    plagiarismWords.insert("лес");
    plagiarismWords.insert("река");
    plagiarismWords.insert("работа");
    plagiarismWords.insert("учеба");
    plagiarismWords.insert("компьютер");
    plagiarismWords.insert("программа");
    plagiarismWords.insert("алгоритм");
    return plagiarismWords;
}

void MainWindow::updateAnalysisResults(double uniqueness, int totalChars, int charsNoSpaces, int wordCount) {
    ui->lblUniqueness->setText(QString("Уникальность: %1%").arg(uniqueness, 0, 'f', 2));
    ui->progressBar->setValue(static_cast<int>(uniqueness));
    ui->lblChars->setText(QString("Всего символов: %1").arg(totalChars));
    ui->lblNoSpaces->setText(QString("Без пробелов: %1").arg(charsNoSpaces));
    ui->lblWords->setText(QString("Количество слов: %1").arg(wordCount));
}

void MainWindow::on_btnSave_clicked() {
    double uniqueness = ui->lblUniqueness->text().remove("Уникальность: ").remove("%").toDouble();
    int totalChars = ui->lblChars->text().remove("Всего символов: ").toInt();
    int charsNoSpaces = ui->lblNoSpaces->text().remove("Без пробелов: ").toInt();
    int wordCount = ui->lblWords->text().remove("Количество слов: ").toInt();

    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить отчет", "", "Текстовые файлы (*.txt);;Все файлы (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QTextStream out(&file);
    out << getReport(uniqueness, totalChars, charsNoSpaces, wordCount);
    file.close();

    QMessageBox::information(this, "Сохранено", "Отчет успешно сохранен");
}

QString MainWindow::getReport(double uniqueness, int totalChars, int charsNoSpaces, int wordCount) const {
    QString report;
    report += "Отчет анализа текста\n";
    report += "-------------------\n";
    report += QString("Дата анализа: %1\n").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));
    report += QString("Уникальность: %1%\n").arg(uniqueness, 0, 'f', 2);
    report += QString("Всего символов: %1\n").arg(totalChars);
    report += QString("Символов без пробелов: %1\n").arg(charsNoSpaces);
    report += QString("Количество слов: %1\n").arg(wordCount);
    report += "\nПараметры анализа:\n";
    report += QString("Игнорировать регистр: %1\n").arg(ui->chkIgnoreCase->isChecked() ? "Да" : "Нет");

    return report;
}

void MainWindow::on_btnSaveSettings_clicked() {
    saveSettings();
    QMessageBox::information(this, "Сохранено", "Настройки успешно сохранены");
}

void MainWindow::loadSettings() {
    QSettings settings("MyCompany", "PlagiarismAnalyzer");
    ui->chkIgnoreCase->setChecked(settings.value("ignoreCase", true).toBool());
}

void MainWindow::saveSettings() {
    QSettings settings("MyCompany", "PlagiarismAnalyzer");
    settings.setValue("ignoreCase", ui->chkIgnoreCase->isChecked());
}

void MainWindow::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Текстовые файлы (*.txt);;Все файлы (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QTextStream in(&file);
    ui->textEdit->setPlainText(in.readAll());
    file.close();
}

void MainWindow::on_actionSave_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить текст", "", "Текстовые файлы (*.txt);;Все файлы (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();
    file.close();
}

void MainWindow::on_actionExit_triggered() {
    close();
}

void MainWindow::on_actionAbout_triggered() {
    QMessageBox::about(this, "О программе",
                       "Анализатор плагиата\n"
                       "Версия 1.0\n\n"
                       "Программа для анализа уникальности текста.\n"
                       "Позволяет определить процент плагиата в тексте.");
}
