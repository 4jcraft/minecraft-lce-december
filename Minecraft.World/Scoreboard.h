#pragma once

class Objective;
class ObjectiveCriteria;
class PlayerTeam;
class Score;

class Scoreboard {
public:
    static const int DISPLAY_SLOT_LIST = 0;
    static const int DISPLAY_SLOT_SIDEBAR = 1;
    static const int DISPLAY_SLOT_BELOW_NAME = 2;
    static const int DISPLAY_SLOTS = 3;

private:
    unordered_map<std::wstring, Objective*> objectivesByName;
    unordered_map<ObjectiveCriteria*, vector<Objective*>*> objectivesByCriteria;
    unordered_map<std::wstring, unordered_map<Objective*, Score*> >
        playerScores;
    Objective* displayObjectives[DISPLAY_SLOTS];
    unordered_map<std::wstring, PlayerTeam*> teamsByName;
    unordered_map<std::wstring, PlayerTeam*> teamsByPlayer;

public:
    Objective* getObjective(const std::wstring& name);
    Objective* addObjective(const std::wstring& name,
                            ObjectiveCriteria* criteria);
    vector<Objective*>* findObjectiveFor(ObjectiveCriteria* criteria);
    Score* getPlayerScore(const std::wstring& name, Objective* objective);
    vector<Score*>* getPlayerScores(Objective* objective);
    vector<Objective*>* getObjectives();
    vector<std::wstring>* getTrackedPlayers();
    void resetPlayerScore(const std::wstring& player);
    vector<Score*>* getScores();
    vector<Score*>* getScores(Objective* objective);
    unordered_map<Objective*, Score*>* getPlayerScores(
        const std::wstring& player);
    void removeObjective(Objective* objective);
    void setDisplayObjective(int slot, Objective* objective);
    Objective* getDisplayObjective(int slot);
    PlayerTeam* getPlayerTeam(const std::wstring& name);
    PlayerTeam* addPlayerTeam(const std::wstring& name);
    void removePlayerTeam(PlayerTeam* team);
    void addPlayerToTeam(const std::wstring& player, PlayerTeam* team);
    bool removePlayerFromTeam(const std::wstring& player);
    void removePlayerFromTeam(const std::wstring& player, PlayerTeam* team);
    vector<std::wstring>* getTeamNames();
    vector<PlayerTeam*>* getPlayerTeams();
    std::shared_ptr<Player> getPlayer(const std::wstring& name);
    PlayerTeam* getPlayersTeam(const std::wstring& name);
    void onObjectiveAdded(Objective* objective);
    void onObjectiveChanged(Objective* objective);
    void onObjectiveRemoved(Objective* objective);
    void onScoreChanged(Score* score);
    void onPlayerRemoved(const std::wstring& player);
    void onTeamAdded(PlayerTeam* team);
    void onTeamChanged(PlayerTeam* team);
    void onTeamRemoved(PlayerTeam* team);
    static std::wstring getDisplaySlotName(int slot);
    static int getDisplaySlotByName(const std::wstring& name);
};