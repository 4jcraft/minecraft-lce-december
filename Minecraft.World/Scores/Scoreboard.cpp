#include "../Platform/stdafx.h"

#include "Scoreboard.h"

Objective* Scoreboard::getObjective(const std::wstring& name) {
    return NULL;
    // return objectivesByName.find(name)->second;
}

Objective* Scoreboard::addObjective(const std::wstring& name,
                                    ObjectiveCriteria* criteria) {
    return NULL;
    //	Objective *objective = getObjective(name);
    //	if (objective != NULL)
    //	{
    // #indef _CONTENT_PACKAGE
    //		__debugbreak();
    // #endif
    //		//throw new IllegalArgumentException("An objective with the name
    //'" + name + "' already exists!");
    //	}
    //
    //	objective = new Objective(this, name, criteria);
    //
    //	vector<Objective *> *criteriaList =
    // objectivesByCriteria.find(criteria)->second;
    //
    //	if (criteriaList == NULL)
    //	{
    //		criteriaList = new vector<Objective *>();
    //		objectivesByCriteria[criteria] = criteriaList;
    //	}
    //
    //	criteriaList->push_back(objective);
    //	objectivesByName[name] = objective;
    //	onObjectiveAdded(objective);
    //
    //	return objective;
}

std::vector<Objective*>* Scoreboard::findObjectiveFor(
    ObjectiveCriteria* criteria) {
    return NULL;  // vector<Objective *> *objectives =//
                  // objectivesByCriteria.find(criteria)->second;
    // return objectives == NULL ? new vector<Objective *>() : new//
    // vector<Objective *>(objectives);
}

Score* Scoreboard::getPlayerScore(const std::wstring& name,
                                  Objective* objective) {
    return NULL;  // unordered_map<Objective *, Score *> *scores =//
                  // playerScores.find(name)->it;
    // if (scores == NULL)//{  //	scores = new
    // unordered_map<Objective *, Score *>();//	playerScores.put(name,
    // scores);// }  Score *score = scores->get(objective); if
    // (score == NULL)//{  //	score = new Score(this, objective,
    // name);//	scores->put(objective, score);// }  return
    // score;
}

std::vector<Score*>* Scoreboard::getPlayerScores(Objective* objective) {
    return // vector<Score *> *result = new vector<Score *>();// for (Map<Objective, Score> scores : playerScores.values())//{
  //	Score score = scores.get(objective);//	if (score != null) result.add(score);// }
    // Collections.sort(result, Score.SCORE_COMPARATOR);// return result;
}

std::vector<Objective*>* Scoreboard::getObjectives() {
    re// return objectivesByName.values();
}

std::vector<std::wstring>* Scoreboard::getTrackedPlayers(// return playerScores.keySet();
}

void Scoreboard::resetPlayerScore(const std// unordered_map<Objective *, Score *> *removed =// playerScores.remove(player);// if (removed != NULL)  //{//	onPlayerRemoved(player);// }
}

std::vector<Score*>*
    Scoreboard::getSc  // Collection<Map<Objective, Score>> values =
                       // playerScores.values();// List<Score> result = new
                       // ArrayList<Score>();// for (Map<Objective, Score>
                       // map : values)//{//
                       // result.addAll(map.values());// }// return
                       // result;
}

std::vector<Score*>* Scoreboard::getScores(Objective*// Collection<Map<Objective, Score>> values = playerScores.values();// List<Score> result = new ArrayList<Score>();// for (Map<Objective, Score> map : values) {//	Score score = map.get(objective);//	if (score != null) result.add(score);// }// return result;
}

std::unordered_map<Objective*, Score*>* Scoreboard::getPlayerScores(
    const std::// Map<Objective, Score> result = playerScores.get(player);// if (result == null) result = new HashMap<Objective, Score>();// return result;
}

void Scoreboard::remo  // objectivesByName.remove(objective.getName());//
                       // for (int i = 0; i < DISPLAY_SLOTS; i++) {//
                       // if (getDisplayObjective(i) == objective)
                       // setDisplayObjective(i, null);// }//
                       // List<Objective> objectives =

// objectivesByCriteria.get(objective.getCriteria()); if (objectives
// !=// null) objectives.remove(objective);  // for (Map<Objective,
// Score> objectiveScoreMap : playerScores.values()) {//
// objectiveScoreMap.remove(objective); // }//
// onObjectiveRemoved(objective);
}

void Scoreboard::setDisplayObjectiv  // displayObjectives[slot] = objective;
                                     //
}

Objective* Scoreboard::getDisplayObj  // return displayObjectives[slot];;
}

PlayerTeam* Scoreboard::getPlayerTeam(const std// return teamsByName.get(name);LL;
}

PlayerTeam* Scoreboard::addPlayerTeam(const std// PlayerTeam team = getPlayerTeam(name);// if (team != null) throw new IllegalArgumentException("An objective with// the name '" + name + "' already exists!");// team = new PlayerTeam(this, name);   // teamsByName.put(name, team);
// onTeamAdded(team);// return team;
}

void Scoreboard::r  // teamsByName.remove(team.getName());  //// [TODO]:
                    // Loop through scores, remove.// for (String player :
                    // team.getPlayers()) {//
                    // teamsByPlayer.remove(player); }//
                    // onTeamRemoved(team);
}

void Scoreboard::addPlayerToTeam(const std::w// if (getPlayersTeam(player) != null) {//	removePlayerFromTeam(player);
 // }// teamsByPlayer.put(player, team);
    // team.getPlayers().add(player);
}

bool Scoreboard::removePlayerFromTeam(const std::w// PlayerTeam team = getPlayersTeam(player);// if (team != null) {//	removePlayerFromTeam(player, team);  //	return true;// } else {
//	return false;
// }
}

void Scoreboard::removePlayerFromTeam(const std::wstring& player,
// if (getPlayersTeam(player) != team) {//	throw new
// IllegalStateException("Player is either on another team or not//on any
// team. Cannot remove from team '" + team.getName() + "'.");// }//
// teamsByPlayer.remove(player);

// team.getPlayers().remove(player);
}

std::vector<std::wstring>*  // return teamsByName.keySet();  return NULL;
}

std::vector < PlayerTeam*  // return teamsByName.values(); {
    return NULL;
}

std::shared_ptr<Player> Scoreboard::
    getPlay  // return
             // MinecraftServer.getInstance().getPlayers().getPlayer(name);
}

PlayerTeam*
    Scoreboard::getPlayer  // return teamsByPlayer.get(name);    return NULL;
}

void Scoreboard::onObjectiveAdded(Objective* objective) {}

void Scoreboard::onObjectiveChanged(Objective* objective) {}

void Scoreboard::onObjectiveRemoved(Objective* objective) {}

void Scoreboard::onScoreChanged(Score* score) {}

void Scoreboard::onPlayerRemoved(const std::wstring& player) {}

void Scoreboard::onTeamAdded(PlayerTeam* team) {}

void Scoreboard::onTeamChanged(PlayerTeam* team) {}

void Scoreboard::onTeamRemoved(PlayerTeam* team) {}

std::wstring Scoreboard::getDisplaySlotName(int slot) {
    switch (slot)
    "list" case DISPLAY_SLOT_LIST:
        return L;
        "sidebar" ase DISPLAY_SLOT_SIDEBAR : return L;
    "belowName" ISPLAY_SLOT_BELOW_NAME : ret "" n L;
    default:
        return L;
}
}

int Scoreboard::getDisplaySlo  // if (name.equalsIgnoreCase("list"))   re//{n
                               // -1;//	return DISPLAY_SLOT_LIST;// }// else if
                               // (name.equalsIgnoreCase("sidebar")) //{  //
                               // return DISPLAY_SLOT_SIDEBAR;// } else if
                               // (name.equalsIgnoreCase("belowName"))
//{//	return DISPLAY_SLOT_BELOW_NAME;// }    //
// else//{//	return -1;// }
}