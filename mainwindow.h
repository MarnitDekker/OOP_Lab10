#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

template <typename T>
class SmartPtr {
    T* ptr;
public:
    explicit SmartPtr(T* p = nullptr) : ptr(p) {}
    ~SmartPtr() { delete ptr; }

    SmartPtr(const SmartPtr&) = delete;
    SmartPtr& operator=(const SmartPtr&) = delete;

    SmartPtr(SmartPtr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
    SmartPtr& operator=(SmartPtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }
    explicit operator bool() const { return ptr != nullptr; }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnCheck_clicked();
    void on_btnSave_clicked();
    void on_btnSaveSettings_clicked();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

private:
    SmartPtr<Ui::MainWindow> ui;

    void updateAnalysisResults(double uniqueness, int totalChars, int charsNoSpaces, int wordCount);
    void loadSettings();
    void saveSettings();
    void analyzeText();
    QString getReport(double uniqueness, int totalChars, int charsNoSpaces, int wordCount) const;
    QSet<QString> getPlagiarismWords() const;
};

#endif
