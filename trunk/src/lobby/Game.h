#pragma once

#include <string>
using std::string;

namespace TLMP {
  namespace Network {
    namespace Lobby {

      class Game {
      public:
        Game();
        ~Game();

        void setName(string name);
        void setDescription(string description);
        void setCurrentLevel(string level);
        void setMaxPlayers(int max_players);
        void setCurrentPlayers(int current_players);
        void setID(int id);
        void setPort(int port);

        string getName() const;
        string getDescription() const;
        string getCurrentLevel() const;
        int getMaxPlayers() const;
        int getCurrentPlayers() const;
        int getID() const;
        int getPort() const;

      private:
        string m_name, m_description, m_currentLevel;
        int m_maxPlayers, m_currentPlayers, m_ID, m_port;
      };

    };
  };
};
