
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <pqxx/pqxx>

#include <memory>
#include <mutex>
#include <string>

namespace Core::Common::Postgres {

/**
 * This class holds connection credentials and the strategy on how to deal with multiple connections.
 * A PostgreSQL connection can then be opened using those credentials with getConnection().
 */
class Connection
{
public:
    enum class Strategy {
        globalLocked,    ///< open a global connection (next connection request will block until the current connection is no longer used)
        globalUnlocked,  ///< open a global connection (without locking) (should be preferred if you only need a single connection)
        local,           ///< open a new local connection
    };

    Connection(Strategy strategy, std::string const host, unsigned short const port, std::string const dbName, std::string const dbUser, std::string const dbPass)
      : strategy_(strategy), host_(host), port_(port), dbName_(dbName), dbUser_(dbUser), dbPass_(dbPass)
    {
    }

    std::shared_ptr<pqxx::connection> getConnection()
    {
        switch (strategy_)
        {
            case Strategy::globalLocked:
                mutex_.lock();
                ensureConnection();
                return std::shared_ptr<pqxx::connection>(connection_.get(), [&](auto p [[gnu::unused]]) { mutex_.unlock(); });
            case Strategy::globalUnlocked: ensureConnection(); return connection_;
            case Strategy::local:
                ensureConnection();
                auto c = connection_;
                connection_.reset();
                return c;
        }
        return nullptr;
    }

private:
    Strategy strategy_;
    std::shared_ptr<pqxx::connection> connection_;
    std::mutex mutex_;
    std::string const host_;
    unsigned short const port_;
    std::string const dbName_;
    std::string const dbUser_;
    std::string const dbPass_;

    void ensureConnection()
    {
        if (!connection_)
        {
            std::string dbConfigStr = "host='" + host_ + "' port=" + std::to_string(port_) + " dbname='" + dbName_ + "' user='" + dbUser_ + "' password='" + dbPass_ + "'";
            connection_ = std::make_shared<pqxx::connection>(dbConfigStr);
        }
    }
};

}  // namespace Core::Common::Postgres
