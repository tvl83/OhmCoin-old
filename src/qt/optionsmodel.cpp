#include "optionsmodel.h"

#include "bitcoinunits.h"
#include "init.h"
#include "wallet.h"
#include "walletdb.h"
#include "guiutil.h"

#include <QSettings>

bool fUseBlackTheme;

OptionsModel::OptionsModel(QObject *parent) :
    QAbstractListModel(parent)
{
    Init();
}

bool static ApplyProxySettings()
{
    QSettings settings;
    CService addrProxy(settings.value("addrProxy", "127.0.0.1:9050").toString().toStdString());
    if (!settings.value("fUseProxy", false).toBool()) {
        addrProxy = CService();
        return false;
    }
    if (!addrProxy.IsValid())
        return false;
    if (!IsLimited(NET_IPV4))
        SetProxy(NET_IPV4, addrProxy);
    if (!IsLimited(NET_IPV6))
        SetProxy(NET_IPV6, addrProxy);
    SetNameProxy(addrProxy);
    return true;
}

void OptionsModel::Init()
{
    QSettings settings;

    // These are Qt-only settings:
    nDisplayUnit = settings.value("nDisplayUnit", BitcoinUnits::BTC).toInt();
    fMinimizeToTray = settings.value("fMinimizeToTray", false).toBool();
    fMinimizeOnClose = settings.value("fMinimizeOnClose", false).toBool();
    fCoinControlFeatures = settings.value("fCoinControlFeatures", false).toBool();
    nTransactionFee = settings.value("nTransactionFee").toLongLong();
    nReserveBalance = settings.value("nReserveBalance").toLongLong();
    language = settings.value("language", "").toString();
    fUseBlackTheme = settings.value("fUseBlackTheme", false).toBool();

    if (!settings.contains("nDarksendRounds"))
        settings.setValue("nDarksendRounds", 2);

    if (!settings.contains("nAnonymizeOhmcoinAmount"))
        settings.setValue("nAnonymizeOhmcoinAmount", 1000);

    nDarksendRounds = settings.value("nDarksendRounds").toLongLong();
    nAnonymizeOhmcoinAmount = settings.value("nAnonymizeOhmcoinAmount").toLongLong();

    // These are shared with core Bitcoin; we want
    // command-line options to override the GUI settings:
    if (settings.contains("fUseUPnP"))
        SoftSetBoolArg("-upnp", settings.value("fUseUPnP").toBool());
    if (settings.contains("addrProxy") && settings.value("fUseProxy").toBool())
        SoftSetArg("-proxy", settings.value("addrProxy").toString().toStdString());
    if (settings.contains("fMinimizeCoinAge"))
        SoftSetBoolArg("-minimizecoinage", settings.value("fMinimizeCoinAge").toBool());
    if (!language.isEmpty())
        SoftSetArg("-lang", language.toStdString());

    if (settings.contains("nDarksendRounds"))
        SoftSetArg("-darksendrounds", settings.value("nDarksendRounds").toString().toStdString());
    if (settings.contains("nAnonymizeOhmcoinAmount"))
        SoftSetArg("-anonymizeOhmcoinamount", settings.value("nAnonymizeOhmcoinAmount").toString().toStdString());
}

int OptionsModel::rowCount(const QModelIndex & parent) const
{
    return OptionIDRowCount;
}

QVariant OptionsModel::data(const QModelIndex & index, int role) const
{
    if(role == Qt::EditRole)
    {
        QSettings settings;
        switch(index.row())
        {
        case StartAtStartup:
            return QVariant(GUIUtil::GetStartOnSystemStartup());
        case MinimizeToTray:
            return QVariant(fMinimizeToTray);
        case MapPortUPnP:
            return settings.value("fUseUPnP", GetBoolArg("-upnp", true));
        case MinimizeOnClose:
            return QVariant(fMinimizeOnClose);
        case ProxyUse:
            return settings.value("fUseProxy", false);
        case ProxyIP: {
            proxyType proxy;
            if (GetProxy(NET_IPV4, proxy))
                return QVariant(QString::fromStdString(proxy.ToStringIP()));
            else
                return QVariant(QString::fromStdString("127.0.0.1"));
        }
        case ProxyPort: {
            proxyType proxy;
            if (GetProxy(NET_IPV4, proxy))
                return QVariant(proxy.GetPort());
            else
                return QVariant(9050);
        }
        case Fee:
            return QVariant((qint64) nTransactionFee);
        case ReserveBalance:
            return QVariant((qint64) nReserveBalance);
        case DisplayUnit:
            return QVariant(nDisplayUnit);
        case Language:
            return settings.value("language", "");
        case CoinControlFeatures:
            return QVariant(fCoinControlFeatures);
        case MinimizeCoinAge:
            return settings.value("fMinimizeCoinAge", GetBoolArg("-minimizecoinage", false));
        case UseBlackTheme:
            return QVariant(fUseBlackTheme);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool OptionsModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    bool successful = true; /* set to false on parse error */
    if(role == Qt::EditRole)
    {
        QSettings settings;
        switch(index.row())
        {
        case StartAtStartup:
            successful = GUIUtil::SetStartOnSystemStartup(value.toBool());
            break;
        case MinimizeToTray:
            fMinimizeToTray = value.toBool();
            settings.setValue("fMinimizeToTray", fMinimizeToTray);
            break;
        case MapPortUPnP:
            settings.setValue("fUseUPnP", value.toBool());
            MapPort(value.toBool());
            break;
        case MinimizeOnClose:
            fMinimizeOnClose = value.toBool();
            settings.setValue("fMinimizeOnClose", fMinimizeOnClose);
            break;
        case ProxyUse:
            settings.setValue("fUseProxy", value.toBool());
            ApplyProxySettings();
            break;
        case ProxyIP: {
            proxyType proxy;
            proxy = CService("127.0.0.1", 9050);
            GetProxy(NET_IPV4, proxy);

            CNetAddr addr(value.toString().toStdString());
            proxy.SetIP(addr);
            settings.setValue("addrProxy", proxy.ToStringIPPort().c_str());
            successful = ApplyProxySettings();
        }
        break;
        case ProxyPort: {
            proxyType proxy;
            proxy = CService("127.0.0.1", 9050);
            GetProxy(NET_IPV4, proxy);

            proxy.SetPort(value.toInt());
            settings.setValue("addrProxy", proxy.ToStringIPPort().c_str());
            successful = ApplyProxySettings();
        }
        break;
        case Fee:
            nTransactionFee = value.toLongLong();
            settings.setValue("nTransactionFee", (qint64) nTransactionFee);
            emit transactionFeeChanged(nTransactionFee);
            break;
        case ReserveBalance:
            nReserveBalance = value.toLongLong();
            settings.setValue("nReserveBalance", (qint64) nReserveBalance);
            emit reserveBalanceChanged(nReserveBalance);
            break;
        case DisplayUnit:
            nDisplayUnit = value.toInt();
            settings.setValue("nDisplayUnit", nDisplayUnit);
            emit displayUnitChanged(nDisplayUnit);
            break;
        case Language:
            settings.setValue("language", value);
            break;
        case CoinControlFeatures: {
            fCoinControlFeatures = value.toBool();
            settings.setValue("fCoinControlFeatures", fCoinControlFeatures);
            emit coinControlFeaturesChanged(fCoinControlFeatures);
            }
            break;
        case MinimizeCoinAge:
           fMinimizeCoinAge = value.toBool();
           settings.setValue("fMinimizeCoinAge", fMinimizeCoinAge);
           break;
        case UseBlackTheme:
            fUseBlackTheme = value.toBool();
            settings.setValue("fUseBlackTheme", fUseBlackTheme);
            break;
        case DarksendRounds:
            nDarksendRounds = value.toInt();
            settings.setValue("nDarksendRounds", nDarksendRounds);
            emit darksendRoundsChanged(nDarksendRounds);
            break;
        case anonymizeOhmcoinAmount:
            nAnonymizeOhmcoinAmount = value.toInt();
            settings.setValue("nAnonymizeOhmcoinAmount", nAnonymizeOhmcoinAmount);
            emit anonymizeOhmcoinAmountChanged(nAnonymizeOhmcoinAmount);
            break;
        default:
            break;
        }
    }
    emit dataChanged(index, index);

    return successful;
}

qint64 OptionsModel::getTransactionFee()
{
    return nTransactionFee;
}

qint64 OptionsModel::getReserveBalance()
{
    return nReserveBalance;
}

bool OptionsModel::getCoinControlFeatures()
{
    return fCoinControlFeatures;
}

bool OptionsModel::getMinimizeToTray()
{
    return fMinimizeToTray;
}

bool OptionsModel::getMinimizeOnClose()
{
    return fMinimizeOnClose;
}

int OptionsModel::getDisplayUnit()
{
    return nDisplayUnit;
}
