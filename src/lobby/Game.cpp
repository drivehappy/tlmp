#include "Game.h"
using namespace TLMP::Network::Lobby;

Game::Game()
{
}

Game::~Game()
{
}


void Game::setName(string name)
{
  m_name = name;
}

void Game::setDescription(string description)
{
  m_description = description;
}

void Game::setMaxPlayers(int max_players)
{
  m_maxPlayers = max_players;
}

void Game::setCurrentPlayers(int current_players)
{
  m_currentPlayers = current_players;
}

void Game::setCurrentLevel(string level)
{
  m_currentLevel = level;
}

void Game::setID(int id)
{
  m_ID = id;
}

void Game::setPort(int port)
{
  m_port = port;
}


string Game::getName() const
{
  return m_name;
}

string Game::getDescription() const
{
  return m_description;
}

int Game::getMaxPlayers() const
{
  return m_maxPlayers;
}

int Game::getCurrentPlayers() const
{
  return m_currentPlayers;
}

string Game::getCurrentLevel() const
{
  return m_currentLevel;
}

int Game::getID() const
{
  return m_ID;
}

int Game::getPort() const
{
  return m_port;
}
