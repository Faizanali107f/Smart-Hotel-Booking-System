// ============================================================
//  MainWindow.cpp  -  implementation of the Qt GUI
// ============================================================
#include "MainWindow.h"
#include "InputValidator.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFontDatabase>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>

#include <stdexcept>

// ======================== small helpers ========================

QString MainWindow::qs(const std::string& s) { return QString::fromStdString(s); }
std::string MainWindow::ss(const QString& s) { return s.toStdString(); }

QString MainWindow::money(double amount) {
    return "PKR " + QLocale(QLocale::English).toString(qRound64(amount));
}

static QTableWidgetItem* cell(const QString& text, bool alignRight = false) {
    QTableWidgetItem* item = new QTableWidgetItem(text);
    if (alignRight)
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    return item;
}

// Adds a page title and a short subtitle at the top of a page
static void addPageHeader(QVBoxLayout* layout, const QString& title, const QString& subtitle) {
    QLabel* t = new QLabel(title);
    t->setObjectName("pageTitle");
    QLabel* s = new QLabel(subtitle);
    s->setObjectName("pageSubtitle");
    layout->addWidget(t);
    layout->addWidget(s);
    layout->addSpacing(8);
}

// A white rounded panel used to group widgets
static QFrame* makeCard() {
    QFrame* card = new QFrame;
    card->setObjectName("card");
    return card;
}

QComboBox* MainWindow::makeCityBox(bool withAll) {
    QComboBox* box = new QComboBox;
    if (withAll) box->addItem("All cities");
    for (int i = 0; i < HotelManager::CITY_COUNT; i++)
        box->addItem(qs(HotelManager::CITIES[i]));
    return box;
}

QTableWidget* MainWindow::makeTable(const QStringList& headers) {
    QTableWidget* table = new QTableWidget(0, headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->verticalHeader()->setVisible(false);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    return table;
}

QLabel* MainWindow::makeErrorLabel() {
    QLabel* label = new QLabel;
    label->setObjectName("errorLabel");
    label->setWordWrap(true);
    return label;
}

QWidget* MainWindow::makeMetric(const QString& title, QLabel*& valueLabel) {
    QFrame* box = new QFrame;
    box->setObjectName("metric");
    QVBoxLayout* l = new QVBoxLayout(box);
    QLabel* t = new QLabel(title);
    t->setObjectName("metricTitle");
    valueLabel = new QLabel("0");
    valueLabel->setObjectName("metricValue");
    l->addWidget(t);
    l->addWidget(valueLabel);
    return box;
}

// ========================== constructor ==========================

MainWindow::MainWindow(HotelManager& manager, QWidget* parent)
    : QMainWindow(parent), manager(manager),
      admin("Hotel Manager", "admin", "admin123"), loginAttempts(0) {

    setWindowTitle("Smart Hotel Booking & Management System");
    resize(1180, 740);

    // ---- sidebar ----
    QWidget* side = new QWidget;
    side->setObjectName("sidebar");
    side->setFixedWidth(210);
    QVBoxLayout* sideLayout = new QVBoxLayout(side);
    sideLayout->setContentsMargins(12, 18, 12, 18);

    QLabel* brand = new QLabel("Smart Hotel\nBooking System");
    brand->setObjectName("brand");
    sideLayout->addWidget(brand);
    sideLayout->addSpacing(16);

    sidebar = new QListWidget;
    sidebar->setObjectName("nav");
    sidebar->addItem("Hotel directory");
    sidebar->addItem("Book a room");
    sidebar->addItem("My booking");
    sidebar->addItem("Cancel booking");
    sidebar->addItem("Admin portal");
    sideLayout->addWidget(sidebar, 1);

    QLabel* footer = new QLabel("CSL-210 OOP Lab Project");
    footer->setObjectName("sideFooter");
    sideLayout->addWidget(footer);

    // ---- pages ----
    pages = new QStackedWidget;
    pages->addWidget(buildDirectoryPage());   // 0
    pages->addWidget(buildBookingPage());     // 1
    pages->addWidget(buildLookupPage());      // 2
    pages->addWidget(buildCancelPage());      // 3
    pages->addWidget(buildAdminPage());       // 4

    QWidget* central = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(side);
    mainLayout->addWidget(pages, 1);
    setCentralWidget(central);

    connect(sidebar, &QListWidget::currentRowChanged, pages, &QStackedWidget::setCurrentIndex);
    sidebar->setCurrentRow(0);

    setStyleSheet(R"(
        QMainWindow, QStackedWidget > QWidget { background: #f4f6fa; }
        #sidebar { background: #1b365d; }
        #brand { color: white; font-size: 17px; font-weight: bold; padding: 4px 8px; }
        #sideFooter { color: #9fb3cf; font-size: 11px; padding: 4px 8px; }
        #nav { background: transparent; border: none; color: #d6e0ee; font-size: 14px; outline: 0; }
        #nav::item { padding: 10px 12px; border-radius: 6px; margin: 2px 0; }
        #nav::item:hover { background: #24497b; }
        #nav::item:selected { background: #2f6fb5; color: white; }
        #pageTitle { font-size: 22px; font-weight: bold; color: #1b365d; }
        #pageSubtitle { font-size: 13px; color: #5b6573; }
        #card { background: white; border: 1px solid #dde3ec; border-radius: 10px; }
        #errorLabel { color: #c0392b; font-size: 12px; }
        #totalLabel { font-size: 20px; font-weight: bold; color: #1b365d; }
        #hint { color: #5b6573; font-size: 12px; }
        #metric { background: white; border: 1px solid #dde3ec; border-radius: 10px; }
        #metricTitle { color: #5b6573; font-size: 12px; }
        #metricValue { color: #1b365d; font-size: 20px; font-weight: bold; }
        QTableWidget { background: white; border: 1px solid #dde3ec; border-radius: 8px;
                       alternate-background-color: #f8fafc; selection-background-color: #dbe9f8;
                       selection-color: #1b365d; }
        QHeaderView::section { background: #eef2f7; color: #1b365d; border: none;
                               padding: 6px; font-weight: bold; }
        QLineEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {
            padding: 5px 8px; border: 1px solid #c9d2de; border-radius: 6px; background: white; }
        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus, QComboBox:focus {
            border: 1px solid #2f6fb5; }
        QPushButton { padding: 8px 16px; border: 1px solid #c9d2de; border-radius: 6px;
                      background: white; color: #1b365d; }
        QPushButton:hover { background: #eef2f7; }
        QPushButton#primary { background: #2f6fb5; color: white; border: none; font-weight: bold; }
        QPushButton#primary:hover { background: #255d99; }
        QPushButton#danger { background: #c0392b; color: white; border: none; font-weight: bold; }
        QPushButton#danger:hover { background: #a93226; }
        QTabWidget::pane { border: none; }
    )");

    refreshAll();
}

// ===================== page 1: hotel directory =====================

QWidget* MainWindow::buildDirectoryPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(28, 24, 28, 24);
    addPageHeader(layout, "Hotel directory",
                  "40 hotels in 8 cities with live room availability");

    QHBoxLayout* filter = new QHBoxLayout;
    filter->addWidget(new QLabel("City:"));
    dirCityBox = makeCityBox(true);
    filter->addWidget(dirCityBox);
    filter->addStretch();
    layout->addLayout(filter);

    dirTable = makeTable({"Hotel", "City", "Single free", "Single / night",
                          "Double free", "Double / night", "Scenic view / night"});
    layout->addWidget(dirTable, 1);

    connect(dirCityBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::refreshDirectory);
    return page;
}

void MainWindow::refreshDirectory() {
    int results[HotelManager::MAX_HOTELS];
    int found;
    if (dirCityBox->currentIndex() == 0) {          // all cities
        found = manager.getHotelCount();
        for (int i = 0; i < found; i++) results[i] = i;
    } else {
        found = manager.findHotelsByCity(ss(dirCityBox->currentText()),
                                         results, HotelManager::MAX_HOTELS);
    }

    dirTable->setRowCount(found);
    for (int r = 0; r < found; r++) {
        const Hotel& h = manager.getHotel(results[r]);
        dirTable->setItem(r, 0, cell(qs(h.getName())));
        dirTable->setItem(r, 1, cell(qs(h.getCity())));
        dirTable->setItem(r, 2, cell(QString::number(h.getSingle()->getAvailable()), true));
        dirTable->setItem(r, 3, cell(money(h.getSingle()->getPrice()), true));
        dirTable->setItem(r, 4, cell(QString::number(h.getDouble()->getAvailable()), true));
        dirTable->setItem(r, 5, cell(money(h.getDouble()->getPrice()), true));
        dirTable->setItem(r, 6, cell(money(h.getSceneryPrice()), true));
    }
}

// ======================= page 2: book a room =======================

QWidget* MainWindow::buildBookingPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(28, 24, 28, 24);
    addPageHeader(layout, "Book a room",
                  "Pick a hotel, fill in the guest details and confirm");

    QHBoxLayout* columns = new QHBoxLayout;
    columns->setSpacing(18);

    // ---- left: choose hotel ----
    QVBoxLayout* left = new QVBoxLayout;
    QHBoxLayout* filter = new QHBoxLayout;
    filter->addWidget(new QLabel("City:"));
    bookCityBox = makeCityBox(false);
    filter->addWidget(bookCityBox);
    filter->addStretch();
    left->addLayout(filter);

    bookHotelTable = makeTable({"Hotel", "Single free", "Double free", "Scenic view"});
    left->addWidget(bookHotelTable, 1);
    columns->addLayout(left, 3);

    // ---- right: booking form ----
    QFrame* card = makeCard();
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 16, 18, 16);

    selectedHotelLabel = new QLabel("Select a hotel from the list");
    selectedHotelLabel->setObjectName("pageTitle");
    selectedHotelLabel->setStyleSheet("font-size: 16px;");
    selectedHotelLabel->setWordWrap(true);
    cardLayout->addWidget(selectedHotelLabel);

    QFormLayout* form = new QFormLayout;
    form->setVerticalSpacing(8);

    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Ali Khan");
    cnicEdit = new QLineEdit;
    cnicEdit->setPlaceholderText("35202-1234567-1");
    phoneEdit = new QLineEdit;
    phoneEdit->setPlaceholderText("0300-1234567");

    guestsSpin = new QSpinBox;
    guestsSpin->setRange(1, 20);
    singleSpin = new QSpinBox;
    singleSpin->setRange(0, 0);
    doubleSpin = new QSpinBox;
    doubleSpin->setRange(0, 0);

    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setDisplayFormat("dd-MM-yyyy");
    dateEdit->setCalendarPopup(true);
    dateEdit->setMinimumDate(QDate::currentDate());
    dateEdit->setMaximumDate(QDate(2030, 12, 31));

    nightsSpin = new QSpinBox;
    nightsSpin->setRange(1, 30);
    sceneryCheck = new QCheckBox("Add scenic view");

    form->addRow("Full name", nameEdit);
    form->addRow("CNIC", cnicEdit);
    form->addRow("Phone", phoneEdit);
    form->addRow("Guests", guestsSpin);
    form->addRow("Single rooms", singleSpin);
    form->addRow("Double rooms", doubleSpin);
    form->addRow("Check-in", dateEdit);
    form->addRow("Nights", nightsSpin);
    form->addRow("", sceneryCheck);
    cardLayout->addLayout(form);

    capacityLabel = new QLabel;
    capacityLabel->setObjectName("hint");
    cardLayout->addWidget(capacityLabel);

    QHBoxLayout* totalRow = new QHBoxLayout;
    QLabel* totalTitle = new QLabel("Estimated total");
    totalTitle->setObjectName("hint");
    totalLabel = new QLabel(money(0));
    totalLabel->setObjectName("totalLabel");
    totalRow->addWidget(totalTitle);
    totalRow->addStretch();
    totalRow->addWidget(totalLabel);
    cardLayout->addLayout(totalRow);

    QLabel* discountHint = new QLabel("Double rooms get 10% off for stays of 5+ nights.");
    discountHint->setObjectName("hint");
    cardLayout->addWidget(discountHint);

    bookErrorLabel = makeErrorLabel();
    cardLayout->addWidget(bookErrorLabel);

    QPushButton* confirm = new QPushButton("Confirm booking");
    confirm->setObjectName("primary");
    cardLayout->addWidget(confirm);
    cardLayout->addStretch();

    columns->addWidget(card, 2);
    layout->addLayout(columns, 1);

    connect(bookCityBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::refreshBookingHotels);
    connect(bookHotelTable, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onBookingHotelSelected);
    connect(guestsSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateEstimate);
    connect(singleSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateEstimate);
    connect(doubleSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateEstimate);
    connect(nightsSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateEstimate);
    connect(sceneryCheck, &QCheckBox::toggled, this, &MainWindow::updateEstimate);
    connect(confirm, &QPushButton::clicked, this, &MainWindow::onConfirmBooking);
    return page;
}

void MainWindow::refreshBookingHotels() {
    int keepHotel = selectedBookingHotel();

    int results[HotelManager::MAX_HOTELS];
    int found = manager.findHotelsByCity(ss(bookCityBox->currentText()),
                                         results, HotelManager::MAX_HOTELS);
    bookHotelIndexes.assign(results, results + found);

    bookHotelTable->blockSignals(true);
    bookHotelTable->clearSelection();
    bookHotelTable->setRowCount(found);
    int rowToSelect = -1;
    for (int r = 0; r < found; r++) {
        const Hotel& h = manager.getHotel(results[r]);
        bookHotelTable->setItem(r, 0, cell(qs(h.getName())));
        bookHotelTable->setItem(r, 1, cell(QString::number(h.getSingle()->getAvailable()) +
                                           "  (" + money(h.getSingle()->getPrice()) + ")", true));
        bookHotelTable->setItem(r, 2, cell(QString::number(h.getDouble()->getAvailable()) +
                                           "  (" + money(h.getDouble()->getPrice()) + ")", true));
        bookHotelTable->setItem(r, 3, cell("+" + money(h.getSceneryPrice()), true));
        if (results[r] == keepHotel) rowToSelect = r;
    }
    if (rowToSelect >= 0) bookHotelTable->selectRow(rowToSelect);
    bookHotelTable->blockSignals(false);
    onBookingHotelSelected();
}

int MainWindow::selectedBookingHotel() const {
    int row = bookHotelTable->currentRow();
    if (row < 0 || row >= (int)bookHotelIndexes.size()) return -1;
    if (bookHotelTable->selectedItems().isEmpty()) return -1;
    return bookHotelIndexes[row];
}

void MainWindow::onBookingHotelSelected() {
    int idx = selectedBookingHotel();
    if (idx < 0) {
        selectedHotelLabel->setText("Select a hotel from the list");
        singleSpin->setRange(0, 0);
        doubleSpin->setRange(0, 0);
        sceneryCheck->setText("Add scenic view");
        updateEstimate();
        return;
    }
    const Hotel& h = manager.getHotel(idx);
    selectedHotelLabel->setText(qs(h.getName()) + ", " + qs(h.getCity()));
    singleSpin->setRange(0, h.getSingle()->getAvailable());
    doubleSpin->setRange(0, h.getDouble()->getAvailable());
    sceneryCheck->setText("Add scenic view (+" + money(h.getSceneryPrice()) + " / night)");
    updateEstimate();
}

void MainWindow::updateEstimate() {
    int idx = selectedBookingHotel();
    if (idx < 0) {
        totalLabel->setText(money(0));
        capacityLabel->setText("");
        return;
    }
    const Hotel& h = manager.getHotel(idx);
    int capacity = singleSpin->value() * h.getSingle()->getCapacity()
                 + doubleSpin->value() * h.getDouble()->getCapacity();

    capacityLabel->setText(QString("Selected rooms fit %1 guest(s)").arg(capacity));
    if (guestsSpin->value() > capacity)
        capacityLabel->setStyleSheet("color: #c0392b;");
    else
        capacityLabel->setStyleSheet("");

    double total = manager.estimateTotal(idx, singleSpin->value(), doubleSpin->value(),
                                         nightsSpin->value(), sceneryCheck->isChecked());
    totalLabel->setText(money(total));
}

void MainWindow::onConfirmBooking() {
    bookErrorLabel->clear();
    try {
        int idx = selectedBookingHotel();
        if (idx < 0)
            throw std::invalid_argument("Select a hotel from the list first.");

        // the same validators the console version uses
        std::string name = ss(nameEdit->text().trimmed());
        std::string cnic = ss(cnicEdit->text().trimmed());
        std::string phone = ss(phoneEdit->text().trimmed());
        std::string date = ss(dateEdit->date().toString("dd-MM-yyyy"));
        InputValidator::validateName(name);
        InputValidator::validateCNIC(cnic);
        InputValidator::validatePhone(phone);
        InputValidator::validateDate(date);

        Guest guest(name, cnic, phone, guestsSpin->value());
        manager.addBooking(guest, idx, singleSpin->value(), doubleSpin->value(),
                           nightsSpin->value(), date, sceneryCheck->isChecked());
    } catch (const std::invalid_argument& e) {
        bookErrorLabel->setText(qs(e.what()));
        return;
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Booking failed", qs(e.what()));
        return;
    }

    const Booking& b = manager.getBooking(manager.getBookingCount() - 1);
    showReceipt("Booking confirmed", qs(b.receiptText()));
    resetBookingForm();
    refreshAll();
}

void MainWindow::resetBookingForm() {
    nameEdit->clear();
    cnicEdit->clear();
    phoneEdit->clear();
    guestsSpin->setValue(1);
    singleSpin->setValue(0);
    doubleSpin->setValue(0);
    nightsSpin->setValue(1);
    sceneryCheck->setChecked(false);
    dateEdit->setDate(QDate::currentDate());
    bookErrorLabel->clear();
}

void MainWindow::showReceipt(const QString& title, const QString& text) {
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    QVBoxLayout* l = new QVBoxLayout(&dialog);

    QPlainTextEdit* box = new QPlainTextEdit(text);
    box->setReadOnly(true);
    box->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    box->setMinimumSize(560, 380);
    l->addWidget(box);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    l->addWidget(buttons);
    dialog.exec();
}

// ======================== page 3: my booking ========================

QWidget* MainWindow::buildLookupPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(28, 24, 28, 24);
    addPageHeader(layout, "My booking", "Enter your CNIC to see your receipts");

    QHBoxLayout* row = new QHBoxLayout;
    lookupCnicEdit = new QLineEdit;
    lookupCnicEdit->setPlaceholderText("35202-1234567-1");
    lookupCnicEdit->setMaximumWidth(260);
    QPushButton* search = new QPushButton("Search");
    search->setObjectName("primary");
    row->addWidget(lookupCnicEdit);
    row->addWidget(search);
    row->addStretch();
    layout->addLayout(row);

    lookupErrorLabel = makeErrorLabel();
    layout->addWidget(lookupErrorLabel);

    lookupResult = new QPlainTextEdit;
    lookupResult->setReadOnly(true);
    lookupResult->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    layout->addWidget(lookupResult, 1);

    connect(search, &QPushButton::clicked, this, &MainWindow::onLookupBooking);
    connect(lookupCnicEdit, &QLineEdit::returnPressed, this, &MainWindow::onLookupBooking);
    return page;
}

void MainWindow::onLookupBooking() {
    lookupErrorLabel->clear();
    lookupResult->clear();
    std::string cnic = ss(lookupCnicEdit->text().trimmed());
    try {
        InputValidator::validateCNIC(cnic);
    } catch (const std::invalid_argument& e) {
        lookupErrorLabel->setText(qs(e.what()));
        return;
    }

    int results[HotelManager::MAX_BOOKINGS];
    int found = manager.findBookingsByCnic(cnic, results, HotelManager::MAX_BOOKINGS);
    if (found == 0) {
        lookupResult->setPlainText("No booking found for CNIC " + qs(cnic) + ".");
        return;
    }
    QString text = QString("%1 booking(s) found\n\n").arg(found);
    for (int i = 0; i < found; i++) {
        text += QString("  Booking #%1\n").arg(i + 1);
        text += qs(manager.getBooking(results[i]).receiptText()) + "\n";
    }
    lookupResult->setPlainText(text);
}

// ====================== page 4: cancel booking ======================

QWidget* MainWindow::buildCancelPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(28, 24, 28, 24);
    addPageHeader(layout, "Cancel booking",
                  "Find your bookings by CNIC, then pick one to cancel");

    QHBoxLayout* row = new QHBoxLayout;
    cancelCnicEdit = new QLineEdit;
    cancelCnicEdit->setPlaceholderText("35202-1234567-1");
    cancelCnicEdit->setMaximumWidth(260);
    QPushButton* find = new QPushButton("Find bookings");
    find->setObjectName("primary");
    row->addWidget(cancelCnicEdit);
    row->addWidget(find);
    row->addStretch();
    layout->addLayout(row);

    cancelErrorLabel = makeErrorLabel();
    layout->addWidget(cancelErrorLabel);

    cancelTable = makeTable({"Hotel", "City", "Check-in", "Nights", "Rooms", "Total"});
    layout->addWidget(cancelTable, 1);

    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addStretch();
    cancelButton = new QPushButton("Cancel selected booking");
    cancelButton->setObjectName("danger");
    bottom->addWidget(cancelButton);
    layout->addLayout(bottom);

    connect(find, &QPushButton::clicked, this, &MainWindow::onFindForCancel);
    connect(cancelCnicEdit, &QLineEdit::returnPressed, this, &MainWindow::onFindForCancel);
    connect(cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelBooking);
    return page;
}

void MainWindow::onFindForCancel() {
    cancelErrorLabel->clear();
    cancelTable->setRowCount(0);
    cancelIndexes.clear();

    std::string cnic = ss(cancelCnicEdit->text().trimmed());
    try {
        InputValidator::validateCNIC(cnic);
    } catch (const std::invalid_argument& e) {
        cancelErrorLabel->setText(qs(e.what()));
        return;
    }

    int results[HotelManager::MAX_BOOKINGS];
    int found = manager.findBookingsByCnic(cnic, results, HotelManager::MAX_BOOKINGS);
    if (found == 0) {
        cancelErrorLabel->setText("No booking found for CNIC " + qs(cnic) + ".");
        return;
    }
    cancelIndexes.assign(results, results + found);
    cancelTable->setRowCount(found);
    for (int r = 0; r < found; r++) {
        const Booking& b = manager.getBooking(results[r]);
        cancelTable->setItem(r, 0, cell(qs(b.getHotel()->getName())));
        cancelTable->setItem(r, 1, cell(qs(b.getHotel()->getCity())));
        cancelTable->setItem(r, 2, cell(qs(b.getCheckIn())));
        cancelTable->setItem(r, 3, cell(QString::number(b.getNights()), true));
        cancelTable->setItem(r, 4, cell(QString("%1 single, %2 double")
                                        .arg(b.getSingles()).arg(b.getDoubles())));
        cancelTable->setItem(r, 5, cell(money(b.getTotal()), true));
    }
    cancelTable->selectRow(0);
}

void MainWindow::onCancelBooking() {
    int row = cancelTable->currentRow();
    if (row < 0 || row >= (int)cancelIndexes.size()) {
        cancelErrorLabel->setText("Find your bookings and select one first.");
        return;
    }
    const Booking& b = manager.getBooking(cancelIndexes[row]);
    QString question = "Cancel the booking at " + qs(b.getHotel()->getName()) +
                       " (" + money(b.getTotal()) + ")?\nThe rooms will be released.";
    if (QMessageBox::question(this, "Cancel booking", question) != QMessageBox::Yes)
        return;

    try {
        manager.cancelBooking(cancelIndexes[row]);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Cancel failed", qs(e.what()));
        return;
    }
    refreshAll();
    onFindForCancel();      // search again: the remaining bookings have new positions
    if (cancelIndexes.empty()) cancelErrorLabel->clear();
    QMessageBox::information(this, "Booking cancelled", "Booking cancelled. Rooms have been released.");
}

// ========================= page 5: admin portal =========================

QWidget* MainWindow::buildAdminPage() {
    adminStack = new QStackedWidget;
    adminStack->addWidget(buildAdminLogin());       // 0
    adminStack->addWidget(buildAdminDashboard());   // 1
    return adminStack;
}

QWidget* MainWindow::buildAdminLogin() {
    QWidget* page = new QWidget;
    QVBoxLayout* outer = new QVBoxLayout(page);
    outer->addStretch();

    QFrame* card = makeCard();
    card->setFixedWidth(360);
    QVBoxLayout* l = new QVBoxLayout(card);
    l->setContentsMargins(24, 22, 24, 22);

    QLabel* title = new QLabel("Admin login");
    title->setObjectName("pageTitle");
    QLabel* subtitle = new QLabel("Only hotel administrators can open this portal");
    subtitle->setObjectName("pageSubtitle");
    subtitle->setWordWrap(true);
    l->addWidget(title);
    l->addWidget(subtitle);
    l->addSpacing(8);

    userEdit = new QLineEdit;
    userEdit->setPlaceholderText("Username");
    passEdit = new QLineEdit;
    passEdit->setPlaceholderText("Password");
    passEdit->setEchoMode(QLineEdit::Password);
    l->addWidget(userEdit);
    l->addWidget(passEdit);

    loginErrorLabel = makeErrorLabel();
    l->addWidget(loginErrorLabel);

    QPushButton* login = new QPushButton("Log in");
    login->setObjectName("primary");
    l->addWidget(login);

    QHBoxLayout* center = new QHBoxLayout;
    center->addStretch();
    center->addWidget(card);
    center->addStretch();
    outer->addLayout(center);
    outer->addStretch();

    connect(login, &QPushButton::clicked, this, &MainWindow::onAdminLogin);
    connect(passEdit, &QLineEdit::returnPressed, this, &MainWindow::onAdminLogin);
    return page;
}

QWidget* MainWindow::buildAdminDashboard() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(28, 24, 28, 24);

    QHBoxLayout* top = new QHBoxLayout;
    QVBoxLayout* titles = new QVBoxLayout;
    QLabel* title = new QLabel("Admin portal");
    title->setObjectName("pageTitle");
    adminInfoLabel = new QLabel;
    adminInfoLabel->setObjectName("pageSubtitle");
    titles->addWidget(title);
    titles->addWidget(adminInfoLabel);
    top->addLayout(titles);
    top->addStretch();
    QPushButton* logout = new QPushButton("Log out");
    top->addWidget(logout);
    layout->addLayout(top);
    layout->addSpacing(8);

    QGridLayout* metrics = new QGridLayout;
    metrics->setSpacing(12);
    metrics->addWidget(makeMetric("Total revenue", revenueValue), 0, 0);
    metrics->addWidget(makeMetric("Active bookings", bookingsValue), 0, 1);
    metrics->addWidget(makeMetric("Free single rooms", freeSinglesValue), 0, 2);
    metrics->addWidget(makeMetric("Free double rooms", freeDoublesValue), 0, 3);
    layout->addLayout(metrics);
    layout->addSpacing(8);

    QTabWidget* tabs = new QTabWidget;

    // ---- reservations ----
    reservationsTable = makeTable({"Guest", "CNIC", "Phone", "Hotel", "City",
                                   "Check-in", "Nights", "Total"});
    tabs->addTab(reservationsTable, "Reservations");

    // ---- revenue ----
    revenueTable = makeTable({"City", "Bookings", "Revenue"});
    tabs->addTab(revenueTable, "Revenue by city");

    // ---- tariffs ----
    QWidget* tariffPage = new QWidget;
    QVBoxLayout* tl = new QVBoxLayout(tariffPage);
    QFrame* card = makeCard();
    card->setMaximumWidth(460);
    QFormLayout* form = new QFormLayout(card);
    form->setContentsMargins(18, 16, 18, 16);
    form->setVerticalSpacing(10);

    tariffCityBox = makeCityBox(false);
    tariffHotelBox = new QComboBox;
    singlePriceSpin = new QDoubleSpinBox;
    doublePriceSpin = new QDoubleSpinBox;
    sceneryPriceSpin = new QDoubleSpinBox;
    singlePriceSpin->setRange(1, 500000);
    doublePriceSpin->setRange(1, 500000);
    sceneryPriceSpin->setRange(0, 500000);
    QDoubleSpinBox* spins[3] = {singlePriceSpin, doublePriceSpin, sceneryPriceSpin};
    for (QDoubleSpinBox* s : spins) {
        s->setDecimals(0);
        s->setSingleStep(500);
        s->setPrefix("PKR ");
    }
    QPushButton* saveTariff = new QPushButton("Save prices");
    saveTariff->setObjectName("primary");

    form->addRow("City", tariffCityBox);
    form->addRow("Hotel", tariffHotelBox);
    form->addRow("Single / night", singlePriceSpin);
    form->addRow("Double / night", doublePriceSpin);
    form->addRow("Scenic view / night", sceneryPriceSpin);
    form->addRow("", saveTariff);
    tl->addWidget(card);
    tl->addStretch();
    tabs->addTab(tariffPage, "Room tariffs");

    layout->addWidget(tabs, 1);

    connect(logout, &QPushButton::clicked, this, &MainWindow::onAdminLogout);
    connect(tariffCityBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTariffCityChanged);
    connect(tariffHotelBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTariffHotelChanged);
    connect(saveTariff, &QPushButton::clicked, this, &MainWindow::onSaveTariff);
    return page;
}

void MainWindow::onAdminLogin() {
    if (loginAttempts >= 3) {
        loginErrorLabel->setText("Too many wrong attempts. Restart the app to try again.");
        return;
    }
    if (!admin.login(ss(userEdit->text()), ss(passEdit->text()))) {
        loginAttempts++;
        loginErrorLabel->setText(QString("Wrong username or password (%1 attempt(s) left).")
                                 .arg(3 - loginAttempts));
        passEdit->clear();
        return;
    }

    loginAttempts = 0;
    loginErrorLabel->clear();
    passEdit->clear();

    // polymorphism: Admin used through a Person reference
    const Person& person = admin;
    adminInfoLabel->setText("Logged in as " + qs(person.getName()) +
                            " (" + qs(person.getRole()) + ")");
    refreshAdmin();
    onTariffCityChanged();
    adminStack->setCurrentIndex(1);
}

void MainWindow::onAdminLogout() {
    userEdit->clear();
    passEdit->clear();
    adminStack->setCurrentIndex(0);
}

void MainWindow::refreshAdmin() {
    revenueValue->setText(money(manager.totalRevenue()));
    bookingsValue->setText(QString::number(Booking::getActiveBookings()));
    freeSinglesValue->setText(QString::number(manager.freeSingleRooms()));
    freeDoublesValue->setText(QString::number(manager.freeDoubleRooms()));

    int count = manager.getBookingCount();
    reservationsTable->setRowCount(count);
    for (int r = 0; r < count; r++) {
        const Booking& b = manager.getBooking(r);
        reservationsTable->setItem(r, 0, cell(qs(b.getGuest().getName())));
        reservationsTable->setItem(r, 1, cell(qs(b.getGuest().getCnic())));
        reservationsTable->setItem(r, 2, cell(qs(b.getGuest().getPhone())));
        reservationsTable->setItem(r, 3, cell(qs(b.getHotel()->getName())));
        reservationsTable->setItem(r, 4, cell(qs(b.getHotel()->getCity())));
        reservationsTable->setItem(r, 5, cell(qs(b.getCheckIn())));
        reservationsTable->setItem(r, 6, cell(QString::number(b.getNights()), true));
        reservationsTable->setItem(r, 7, cell(money(b.getTotal()), true));
    }

    revenueTable->setRowCount(HotelManager::CITY_COUNT + 1);
    int totalBookings = 0;
    for (int c = 0; c < HotelManager::CITY_COUNT; c++) {
        const std::string& city = HotelManager::CITIES[c];
        int n = 0;
        for (int i = 0; i < count; i++)
            if (manager.getBooking(i).getHotel()->matches(city)) n++;
        totalBookings += n;
        revenueTable->setItem(c, 0, cell(qs(city)));
        revenueTable->setItem(c, 1, cell(QString::number(n), true));
        revenueTable->setItem(c, 2, cell(money(manager.revenueForCity(city)), true));
    }
    int last = HotelManager::CITY_COUNT;
    QFont bold = revenueTable->font();
    bold.setBold(true);
    QTableWidgetItem* totalItems[3] = {
        cell("Total"), cell(QString::number(totalBookings), true),
        cell(money(manager.totalRevenue()), true)};
    for (int i = 0; i < 3; i++) {
        totalItems[i]->setFont(bold);
        revenueTable->setItem(last, i, totalItems[i]);
    }
}

void MainWindow::onTariffCityChanged() {
    int results[HotelManager::MAX_HOTELS];
    int found = manager.findHotelsByCity(ss(tariffCityBox->currentText()),
                                         results, HotelManager::MAX_HOTELS);
    tariffHotelBox->blockSignals(true);
    tariffHotelBox->clear();
    for (int i = 0; i < found; i++)
        tariffHotelBox->addItem(qs(manager.getHotel(results[i]).getName()), results[i]);
    tariffHotelBox->blockSignals(false);
    onTariffHotelChanged();
}

void MainWindow::onTariffHotelChanged() {
    if (tariffHotelBox->currentIndex() < 0) return;
    const Hotel& h = manager.getHotel(tariffHotelBox->currentData().toInt());
    singlePriceSpin->setValue(h.getSingle()->getPrice());
    doublePriceSpin->setValue(h.getDouble()->getPrice());
    sceneryPriceSpin->setValue(h.getSceneryPrice());
}

void MainWindow::onSaveTariff() {
    if (tariffHotelBox->currentIndex() < 0) return;
    try {
        Hotel& h = manager.getHotel(tariffHotelBox->currentData().toInt());
        h.getSingle()->setPrice(singlePriceSpin->value());
        h.getDouble()->setPrice(doublePriceSpin->value());
        h.setSceneryPrice(sceneryPriceSpin->value());
        manager.save();
        refreshAll();
        QMessageBox::information(this, "Prices saved",
                                 "New prices saved for " + qs(h.getName()) + ".");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Could not save prices", qs(e.what()));
    }
}

// ============================ refresh ============================

void MainWindow::refreshAll() {
    refreshDirectory();
    refreshBookingHotels();
    refreshAdmin();
}
