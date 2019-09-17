/*
 *  SearchEnvironment.h
 *  hog2
 *
 *  Created by Nathan Sturtevant on 5/15/07.
 *  Copyright 2007 Nathan Sturtevant, University of Alberta. All rights reserved.
 *
 */

#ifndef SEARCHENVIRONMENT_H
#define SEARCHENVIRONMENT_H

#include <stdint.h>
#include <vector>
//#include "ReservationProvider.h"
#include <assert.h>
#include "Heuristic.h"


struct Hash64 {
		size_t operator()(const uint64_t &x) const
		{ return (size_t)(x); }
};


template <class state, class action>
class SearchEnvironment : public Heuristic<state> {
public:
	virtual ~SearchEnvironment() {}
	virtual void GetSuccessors(const state &nodeID, std::vector<state> &neighbors) const = 0;
	virtual void GetActions(const state &nodeID, std::vector<action> &actions) const = 0;
	virtual int GetNumSuccessors(const state &stateID) const
	{ std::vector<state> neighbors; GetSuccessors(stateID, neighbors); return (int)neighbors.size(); }

	virtual action GetAction(const state &s1, const state &s2) const;
	virtual void ApplyAction(state &s, action a) const = 0;
	virtual void UndoAction(state &s, action a) const
	{ assert(InvertAction(a)); ApplyAction(s, a); }

	virtual void GetNextState(const state &s1, action a, state &s2) const
	{
		s2 = s1;
		ApplyAction(s2, a);
	};

	virtual bool InvertAction(action &a) const = 0;

	/** Stores the goal for use by single-state HCost. **/
	virtual void StoreGoal(state &s)
	{ bValidSearchGoal = true; searchGoal = s; }

	/** Clears the goal from memory. **/
	virtual void ClearGoal()
	{ bValidSearchGoal = false; }

	/** Returns true if the goal is stored and false otherwise. **/
	virtual bool IsGoalStored() const
	{ return bValidSearchGoal; }

	/** Heuristic value between two arbitrary nodes. **/
	virtual double HCost(const state &node1, const state &node2) const = 0;
	virtual double HCost(const state &node1, const state &node2, double parentHCost) const
	{ return HCost(node1, node2); }
	/** Heuristic value between node and the stored goal. Asserts that the
	 goal is stored **/
	virtual double HCost(const state &node) const
	{ assert(bValidSearchGoal); return HCost(node, searchGoal); }

	virtual double GCost(const state &node1, const state &node2) const = 0;
	virtual double GCost(const state &node, const action &act) const = 0;
	virtual bool GoalTest(const state &node, const state &goal) const = 0;

	/** Goal Test if the goal is stored **/
	virtual bool GoalTest(const state &node) const
	{ return bValidSearchGoal&&(node == searchGoal); }

	virtual uint64_t GetMaxHash() const { return 0; }
	virtual uint64_t GetStateHash(const state &node) const = 0;
	virtual void GetStateFromHash(uint64_t parent, state &s) const { assert(false); }

	virtual uint64_t GetActionHash(action act) const = 0;

	virtual double GetPathLength(std::vector<state> &neighbors);
	virtual double GetPathLength(const state &start, std::vector<action> &neighbors);

	//virtual OccupancyInterface<state,action> *GetOccupancyInfo()
	//{ return 0; }
	//virtual void SetOccupancyInfo(OccupancyInterface<state,action> *)
	//{ }

protected:
	bool bValidSearchGoal;
	state searchGoal;
};


template <class state, class action>
action SearchEnvironment<state,action>::GetAction(const state &s1, const state &s2) const
{
	std::vector<action> a;
	GetActions(s1, a);
	for (int x = 0; x < a.size(); x++)
	{
		state s = s1;
		ApplyAction(s, a[x]);
		if (s == s2)
		{
			return a[x];
		}
	}
	assert(!"No legal move found.");
	action act;
	//return 0;
}

template <class state, class action>
double SearchEnvironment<state,action>::GetPathLength(std::vector<state> &neighbors)
{
	double length = 0;
	for (unsigned int x = 1; x < neighbors.size(); x++)
	{
		length += GCost(neighbors[x-1], neighbors[x]);
	}
	return length;
}

template <class state, class action>
double SearchEnvironment<state,action>::GetPathLength(const state &start, std::vector<action> &neighbors)
{
	state tmp = start;
	double length = 0;
	for (unsigned int x = 0; x < neighbors.size(); x++)
	{
		length += GCost(tmp, neighbors[x]);
		ApplyAction(tmp, neighbors[x]);
	}
	return length;
}


#endif
