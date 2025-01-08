/// \ingroup base
/// \class PathMappingDistance
/// \author Florian Wetzels (wetzels@cs.uni-kl.de)
/// \date 2022.
///
/// This module defines the %PathMappingDistance class that computes distances
/// between two merge trees.
///
/// \b Related \b publication \n
/// "A Deformation-based Edit Distance for Merge Trees" \n
/// Florian Wetzels, Christoph Garth. \n
/// TopoInVis 2022.

#pragma once

#include <set>
#include <vector>

#include <algorithm>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <stack>
#include <tuple>
#include <vector>
#include <boost/dynamic_bitset.hpp>

// ttk common includes
#include "MergeTreeBase.h"
#include <AssignmentAuction.h>
#include <AssignmentExhaustive.h>
#include <AssignmentMunkres.h>
#include <Debug.h>
#include <FTMTree_MT.h>

namespace ttk {

  class PathMappingDistance : virtual public Debug, public MergeTreeBase {

  private:
    int baseMetric_ = 0;
    int assignmentSolverID_ = 0;
    bool squared_ = false;
    bool computeMapping_ = false;
    unsigned int lookahead = 0;
    std::map<ftm::idNode,int> vid_to_seg1;
    std::map<ftm::idNode,int> vid_to_seg2;
    std::map<double,int> val_to_seg1;
    std::map<double,int> val_to_seg2;

    bool preprocess_ = true;
    bool saveTree_ = false;

    template <class dataType>
    inline dataType editCost_Persistence(int n1,
                                         int p1,
                                         int n2,
                                         int p2,
                                         ftm::FTMTree_MT *tree1,
                                         ftm::FTMTree_MT *tree2) {
      if(!vid_to_seg1.empty() and !vid_to_seg2.empty()
         and n1>0 and tree1->getNumberOfChildren(n1)==0
         and n2>0 and tree2->getNumberOfChildren(n2)==0){
        auto val1 = tree1->getValue<dataType>(n1);
        auto val2 = tree2->getValue<dataType>(n2);
        auto s1 = val_to_seg1[val1];
        auto s2 = val_to_seg2[val2];
        if(s1!=s2){
          dataType b1 = tree1->getValue<dataType>(n1);
          dataType d1 = tree1->getValue<dataType>(p1);
          dataType b2 = tree2->getValue<dataType>(n2);
          dataType d2 = tree2->getValue<dataType>(p2);
          dataType del1 = (d1 > b1) ? (d1 - b1) : (b1 - d1);
          dataType del2 = (d2 > b2) ? (d2 - b2) : (b2 - d2);
          return (del1+del2)*2;
        }
      }
      dataType d;
      if(n1 < 0) {
        dataType b1 = tree2->getValue<dataType>(n2);
        dataType d1 = tree2->getValue<dataType>(p2);
        d = (d1 > b1) ? (d1 - b1) : (b1 - d1);
      } else if(n2 < 0) {
        dataType b1 = tree1->getValue<dataType>(n1);
        dataType d1 = tree1->getValue<dataType>(p1);
        d = (d1 > b1) ? (d1 - b1) : (b1 - d1);
      } else {
        dataType b1 = tree1->getValue<dataType>(n1);
        dataType d1 = tree1->getValue<dataType>(p1);
        dataType b2 = tree2->getValue<dataType>(n2);
        dataType d2 = tree2->getValue<dataType>(p2);
        dataType dist1 = (d1 > b1) ? (d1 - b1) : (b1 - d1);
        dataType dist2 = (d2 > b2) ? (d2 - b2) : (b2 - d2);
        d = (dist1 > dist2) ? (dist1 - dist2) : (dist2 - dist1);
      }
      return squared_ ? d * d : d;
    }

    template <class dataType>
    void traceMapping_path(
      ftm::FTMTree_MT *tree1,
      ftm::FTMTree_MT *tree2,
      int curr1,
      int l1,
      int curr2,
      int l2,
      std::vector<std::vector<int>> &predecessors1,
      std::vector<std::vector<int>> &predecessors2,
      int depth1,
      int depth2,
      std::vector<dataType> &memT,
      std::vector<std::pair<dataType,std::vector<std::pair<ftm::idNode, ftm::idNode>>>> &memLA,
      std::vector<std::pair<std::pair<ftm::idNode, ftm::idNode>,
                            std::pair<ftm::idNode, ftm::idNode>>> &mapping) {

      //===============================================================================
      // If both trees not empty, find optimal edit operation
      std::vector<ftm::idNode> children1;
      tree1->getChildren(curr1, children1);
      std::vector<ftm::idNode> children2;
      tree2->getChildren(curr2, children2);
      int parent1 = predecessors1[curr1][predecessors1[curr1].size() - l1];
      int parent2 = predecessors2[curr2][predecessors2[curr2].size() - l2];

      size_t const nn1 = tree1->getNumberOfNodes();
      size_t const nn2 = tree2->getNumberOfNodes();
      size_t const dim1 = 1;
      size_t const dim2 = (nn1 + 1) * dim1;
      size_t const dim3 = (depth1 + 1) * dim2;
      size_t const dim4 = (nn2 + 1) * dim3;

      //---------------------------------------------------------------------------
      // If both trees only have one branch, return edit cost between
      // the two branches
      if(tree1->getNumberOfChildren(curr1) == 0
         and tree2->getNumberOfChildren(curr2) == 0) {
        mapping.emplace_back(
          std::make_pair(curr1, parent1), std::make_pair(curr2, parent2));
        return;
      }
      //---------------------------------------------------------------------------
      // If first tree only has one branch, try all decompositions of
      // second tree
      else if(tree1->getNumberOfChildren(curr1) == 0) {
        for(auto child2_mb : children2) {
          dataType d_
            = memT[curr1 + l1 * dim2 + child2_mb * dim3 + (l2 + 1) * dim4];
          for(auto child2 : children2) {
            if(child2 == child2_mb) {
              continue;
            }
            d_ += memT[nn1 + 0 * dim2 + child2 * dim3 + 1 * dim4];
          }
          if(memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] == d_) {
            traceMapping_path(tree1, tree2, curr1, l1, child2_mb, l2 + 1,
                              predecessors1, predecessors2, depth1, depth2,
                              memT, memLA, mapping);
            return;
          }
        }
      }
      //---------------------------------------------------------------------------
      // If second tree only has one branch, try all decompositions of
      // first tree
      else if(tree2->getNumberOfChildren(curr2) == 0) {
        dataType d = std::numeric_limits<dataType>::max();
        for(auto child1_mb : children1) {
          dataType d_
            = memT[child1_mb + (l1 + 1) * dim2 + curr2 * dim3 + l2 * dim4];
          for(auto child1 : children1) {
            if(child1 == child1_mb) {
              continue;
            }
            d_ += memT[child1 + 1 * dim2 + nn2 * dim3 + 0 * dim4];
          }
          d = std::min(d, d_);
          if(memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] == d_) {
            traceMapping_path(tree1, tree2, child1_mb, l1 + 1, curr2, l2,
                              predecessors1, predecessors2, depth1, depth2,
                              memT, memLA, mapping);
            return;
          }
        }
      }
      //---------------------------------------------------------------------------
      // If both trees have more than one branch, try all decompositions
      // of both trees
      else {
        //-----------------------------------------------------------------------
        // Try all possible main branches of first tree (child1_mb) and
        // all possible main branches of second tree (child2_mb) Then
        // try all possible matchings of subtrees
        if(tree1->getNumberOfChildren(curr1) == 2
           && tree2->getNumberOfChildren(curr2) == 2) {
          int child11 = children1[0];
          int child12 = children1[1];
          int child21 = children2[0];
          int child22 = children2[1];
          if(memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4]
             == memT[child11 + 1 * dim2 + child21 * dim3 + 1 * dim4]
                  + memT[child12 + 1 * dim2 + child22 * dim3 + 1 * dim4]
                  + editCost_Persistence<dataType>(
                    curr1, parent1, curr2, parent2, tree1, tree2)) {
            mapping.emplace_back(
              std::make_pair(curr1, parent1), std::make_pair(curr2, parent2));
            traceMapping_path(tree1, tree2, child11, 1, child21, 1,
                              predecessors1, predecessors2, depth1, depth2,
                              memT, memLA, mapping);
            traceMapping_path(tree1, tree2, child12, 1, child22, 1,
                              predecessors1, predecessors2, depth1, depth2,
                              memT, memLA, mapping);
            return;
          }
          if(memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4]
             == memT[child11 + 1 * dim2 + child22 * dim3 + 1 * dim4]
                  + memT[child12 + 1 * dim2 + child21 * dim3 + 1 * dim4]
                  + editCost_Persistence<dataType>(
                    curr1, parent1, curr2, parent2, tree1, tree2)) {
            mapping.emplace_back(
              std::make_pair(curr1, parent1), std::make_pair(curr2, parent2));
            traceMapping_path(tree1, tree2, child11, 1, child22, 1,
                              predecessors1, predecessors2, depth1, depth2,
                              memT, memLA, mapping);
            traceMapping_path(tree1, tree2, child12, 1, child21, 1,
                              predecessors1, predecessors2, depth1, depth2,
                              memT, memLA, mapping);
            return;
          }
        } else {
          auto f = [&](int r, int c) {
            size_t const c1
              = r < tree1->getNumberOfChildren(curr1) ? children1[r] : nn1;
            size_t const c2
              = c < tree2->getNumberOfChildren(curr2) ? children2[c] : nn2;
            int const l1_ = c1 == nn1 ? 0 : 1;
            int const l2_ = c2 == nn2 ? 0 : 1;
            return memT[c1 + l1_ * dim2 + c2 * dim3 + l2_ * dim4];
          };
          int size = std::max(tree1->getNumberOfChildren(curr1),
                              tree2->getNumberOfChildren(curr2))
                     + 1;
          auto costMatrix = std::vector<std::vector<dataType>>(
            size, std::vector<dataType>(size, 0));
          std::vector<MatchingType> matching;
          for(int r = 0; r < size; r++) {
            for(int c = 0; c < size; c++) {
              costMatrix[r][c] = f(r, c);
            }
          }

          AssignmentSolver<dataType> *assignmentSolver;
          AssignmentExhaustive<dataType> solverExhaustive;
          AssignmentMunkres<dataType> solverMunkres;
          AssignmentAuction<dataType> solverAuction;
          switch(assignmentSolverID_) {
            case 1:
              solverExhaustive = AssignmentExhaustive<dataType>();
              assignmentSolver = &solverExhaustive;
              break;
            case 2:
              solverMunkres = AssignmentMunkres<dataType>();
              assignmentSolver = &solverMunkres;
              break;
            case 0:
            default:
              solverAuction = AssignmentAuction<dataType>();
              assignmentSolver = &solverAuction;
          }
          assignmentSolver->setInput(costMatrix);
          assignmentSolver->setBalanced(true);
          assignmentSolver->run(matching);
          dataType d_ = editCost_Persistence<dataType>(
            curr1, parent1, curr2, parent2, tree1, tree2);
          for(auto m : matching)
            d_ += std::get<2>(m);
          if(memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] == d_) {
            mapping.emplace_back(
              std::make_pair(curr1, parent1), std::make_pair(curr2, parent2));
            for(auto m : matching) {
              int n1 = std::get<0>(m) < tree1->getNumberOfChildren(curr1)
                         ? children1[std::get<0>(m)]
                         : -1;
              int n2 = std::get<1>(m) < tree2->getNumberOfChildren(curr2)
                         ? children2[std::get<1>(m)]
                         : -1;
              if(n1 >= 0 && n2 >= 0)
                traceMapping_path(tree1, tree2, n1, 1, n2, 1, predecessors1,
                                  predecessors2, depth1, depth2, memT, memLA, mapping);
            }
            return;
          }
        }    
        //-----------------------------------------------------------------------
        // Try to look-ahead
        if(memLA.size()>0 and memLA[curr1+curr2*nn1].first>=0){
          dataType d_ = editCost_Persistence<dataType>(
              curr1, parent1, curr2, parent2, tree1, tree2) 
              +memLA[curr1+curr2*nn1].first;
          if(memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] == d_){
            mapping.emplace_back(
                std::make_pair(curr1, parent1), std::make_pair(curr2, parent2));
            for(auto m : memLA[curr1+curr2*nn1].second){
              traceMapping_path(tree1, tree2, m.first, 1, m.second, 1, predecessors1,
                                    predecessors2, depth1, depth2, memT, memLA, mapping);
            }
            return;
          }
        }
        //-----------------------------------------------------------------------
        // Try to continue main branch on one child of first tree and
        // delete all other subtrees Then match continued branch to
        // current branch in second tree
        for(auto child1_mb : children1) {
          dataType d_
            = memT[child1_mb + (l1 + 1) * dim2 + curr2 * dim3 + l2 * dim4];
          for(auto child1 : children1) {
            if(child1 == child1_mb) {
              continue;
            }
            d_ += memT[child1 + 1 * dim2 + nn2 * dim3 + 0 * dim4];
          }
          if(memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] == d_) {
            traceMapping_path(tree1, tree2, child1_mb, l1 + 1, curr2, l2,
                              predecessors1, predecessors2, depth1, depth2,
                              memT, memLA, mapping);
            return;
          }
        }
        //-----------------------------------------------------------------------
        // Try to continue main branch on one child of second tree and
        // delete all other subtrees Then match continued branch to
        // current branch in first tree
        for(auto child2_mb : children2) {
          dataType d_
            = memT[curr1 + l1 * dim2 + child2_mb * dim3 + (l2 + 1) * dim4];
          for(auto child2 : children2) {
            if(child2 == child2_mb) {
              continue;
            }
            d_ += memT[nn1 + 0 * dim2 + child2 * dim3 + 1 * dim4];
          }
          if(memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] == d_) {
            traceMapping_path(tree1, tree2, curr1, l1, child2_mb, l2 + 1,
                              predecessors1, predecessors2, depth1, depth2,
                              memT, memLA, mapping);
            return;
          }
        }
      }
    }

  public:
    PathMappingDistance() {
      this->setDebugMsgPrefix(
        "MergeTreeDistance"); // inherited from Debug: prefix will be printed at
                              // the beginning of every msg
    }
    ~PathMappingDistance() override = default;

    void setBaseMetric(int m) {
      baseMetric_ = m;
    }

    void setAssignmentSolver(int assignmentSolver) {
      assignmentSolverID_ = assignmentSolver;
    }

    void setSquared(bool s) {
      squared_ = s;
    }

    void setComputeMapping(bool m) {
      computeMapping_ = m;
    }

    void setlookahead(int l) {
      lookahead = l;
    }

    void setPreprocess(bool p) {
      preprocess_ = p;
    }

    void setSaveTree(bool save) {
      saveTree_ = save;
    }

    void setVidToSeg(std::map<ftm::idNode,int> m1, std::map<ftm::idNode,int> m2) {
      vid_to_seg1 = m1;
      vid_to_seg2 = m2;
    }

    void setValToSeg(std::map<double,int> m1, std::map<double,int> m2) {
      val_to_seg1 = m1;
      val_to_seg2 = m2;
    }

    template <class dataType>
    dataType computeDistance(
      ftm::FTMTree_MT *tree1,
      ftm::FTMTree_MT *tree2,
      std::vector<std::pair<std::pair<ftm::idNode, ftm::idNode>,
                            std::pair<ftm::idNode, ftm::idNode>>>
        *outputMatching) {

      // compute preorder of both trees (necessary for bottom-up dynamic
      // programming)

      std::vector<std::vector<int>> predecessors1(tree1->getNumberOfNodes());
      std::vector<std::vector<int>> predecessors2(tree2->getNumberOfNodes());
      int const rootID1 = tree1->getRoot();
      int const rootID2 = tree2->getRoot();
      std::vector<int> preorder1(tree1->getNumberOfNodes());
      std::vector<int> preorder2(tree2->getNumberOfNodes());
      std::vector<dataType> scalarDepth1(tree1->getNumberOfNodes());
      std::vector<dataType> totalPersistence1(tree1->getNumberOfNodes());
      std::vector<dataType> scalarDepth2(tree2->getNumberOfNodes());
      std::vector<dataType> totalPersistence2(tree2->getNumberOfNodes());

      int depth1 = 0;
      int depth2 = 0;
      std::stack<int> stack;
      std::stack<int> postorderstack;
      stack.push(rootID1);
      int count = tree1->getNumberOfNodes() - 1;
      while(!stack.empty()) {
        int const nIdx = stack.top();
        stack.pop();
        postorderstack.push(nIdx);
        preorder1[count] = nIdx;
        count--;
        depth1 = std::max((int)predecessors1[nIdx].size(), depth1);
        std::vector<ftm::idNode> children;
        tree1->getChildren(nIdx, children);
        for(int const cIdx : children) {
          stack.push(cIdx);
          predecessors1[cIdx].reserve(predecessors1[nIdx].size() + 1);
          predecessors1[cIdx].insert(predecessors1[cIdx].end(),
                                     predecessors1[nIdx].begin(),
                                     predecessors1[nIdx].end());
          predecessors1[cIdx].push_back(nIdx);
        }
      }
      while(!postorderstack.empty()) {
        int const nIdx = postorderstack.top();
        postorderstack.pop();
        dataType total = 0;
        dataType longest = 0;
        std::vector<ftm::idNode> children;
        tree1->getChildren(nIdx, children);
        auto nv = tree1->getValue<dataType>(nIdx);
        for(int const cIdx : children) {
          auto cv = tree1->getValue<dataType>(cIdx);
          auto pers = nv > cv ? nv-cv : cv-nv;
          total += pers + totalPersistence1[cIdx];
          longest = std::max(longest,pers + scalarDepth1[cIdx]);
        }
        totalPersistence1[nIdx] = total;
        scalarDepth1[nIdx] = longest;
      }
      postorderstack = std::stack<int>();
      stack.push(rootID2);
      count = tree2->getNumberOfNodes() - 1;
      while(!stack.empty()) {
        int const nIdx = stack.top();
        stack.pop();
        postorderstack.push(nIdx);
        preorder2[count] = nIdx;
        count--;
        depth2 = std::max((int)predecessors2[nIdx].size(), depth2);
        std::vector<ftm::idNode> children;
        tree2->getChildren(nIdx, children);
        for(int const cIdx : children) {
          stack.push(cIdx);
          predecessors2[cIdx].reserve(predecessors2[nIdx].size() + 1);
          predecessors2[cIdx].insert(predecessors2[cIdx].end(),
                                     predecessors2[nIdx].begin(),
                                     predecessors2[nIdx].end());
          predecessors2[cIdx].push_back(nIdx);
        }
      }
      while(!postorderstack.empty()) {
        int const nIdx = postorderstack.top();
        postorderstack.pop();
        dataType total = 0;
        dataType longest = 0;
        std::vector<ftm::idNode> children;
        tree2->getChildren(nIdx, children);
        auto nv = tree2->getValue<dataType>(nIdx);
        for(int const cIdx : children) {
          auto cv = tree2->getValue<dataType>(cIdx);
          auto pers = nv > cv ? nv-cv : cv-nv;
          total += pers + totalPersistence2[cIdx];
          longest = std::max(longest,pers + scalarDepth2[cIdx]);
        }
        totalPersistence2[nIdx] = total;
        scalarDepth2[nIdx] = longest;
      }

      dataType tP1 = totalPersistence1[rootID1];
      dataType tP2 = totalPersistence2[rootID2];
      dataType lP1 = scalarDepth1[rootID1];
      dataType lP2 = scalarDepth2[rootID2];
      // dataType globalLowerBound = tP1>tP2 ? tP1-tP2 : tP2-tP1;
      dataType globalUpperBound = lP1>lP2 ? lP1-lP2 : lP2-lP1;
      globalUpperBound += tP1-lP1 + tP2-lP2;

      if(lookahead>0){
        auto lookahead_tmp = lookahead;
        auto computeMapping_tmp = computeMapping_;
        auto preprocess_tmp = preprocess_;
        lookahead = 0;
        computeMapping_ = false;

        auto dist = computeDistance<dataType>(tree1,tree2,outputMatching);
        globalUpperBound = dist;

        preprocess_ = preprocess_tmp;
        computeMapping_ = computeMapping_tmp;
        lookahead = lookahead_tmp;
      }

      // initialize memoization tables

      size_t nn1 = tree1->getNumberOfNodes();
      size_t nn2 = tree2->getNumberOfNodes();
      size_t const dim1 = 1;
      size_t const dim2 = (nn1 + 1) * dim1;
      size_t const dim3 = (depth1 + 1) * dim2;
      size_t const dim4 = (nn2 + 1) * dim3;

      // std::cout << (nn1 + 1) * (depth1 + 1) * (nn2 + 1) * (depth2 + 1) *
      // sizeof(dataType) << std::endl;
      std::vector<dataType> memT((nn1 + 1) * (depth1 + 1) * (nn2 + 1)
                                 * (depth2 + 1));
      std::vector<std::pair<dataType,std::vector<std::pair<ftm::idNode, ftm::idNode>>>> memLA;
      if(lookahead>0 and computeMapping_){
        memLA.resize((nn1) * (nn2),std::make_pair(-1.0,std::vector<std::pair<ftm::idNode, ftm::idNode>>()));
      }

      memT[nn1 + 0 * dim2 + nn2 * dim3 + 0 * dim4] = 0;
      for(size_t i = 0; i < nn1; i++) {
        int curr1 = preorder1[i];
        std::vector<ftm::idNode> children1;
        tree1->getChildren(curr1, children1);
        for(size_t l = 1; l <= predecessors1[preorder1[i]].size(); l++) {
          int parent1 = predecessors1[preorder1[i]]
                                     [predecessors1[preorder1[i]].size() - l];

          //-----------------------------------------------------------------------
          // Delete curr path and full subtree rooted in path
          memT[curr1 + l * dim2 + nn2 * dim3 + 0 * dim4]
            = editCost_Persistence<dataType>(
              curr1, parent1, -1, -1, tree1, tree2);
          for(auto child1 : children1) {
            memT[curr1 + l * dim2 + nn2 * dim3 + 0 * dim4]
              += memT[child1 + 1 * dim2 + nn2 * dim3 + 0 * dim4];
          }
        }
      }
      for(size_t j = 0; j < nn2; j++) {
        int curr2 = preorder2[j];
        std::vector<ftm::idNode> children2;
        tree2->getChildren(curr2, children2);
        for(size_t l = 1; l <= predecessors2[preorder2[j]].size(); l++) {
          int parent2 = predecessors2[preorder2[j]]
                                     [predecessors2[preorder2[j]].size() - l];

          //-----------------------------------------------------------------------
          // Delete curr path and full subtree rooted in path
          memT[nn1 + 0 * dim2 + curr2 * dim3 + l * dim4]
            = editCost_Persistence<dataType>(
              -1, -1, curr2, parent2, tree1, tree2);
          for(auto child2 : children2) {
            memT[nn1 + 0 * dim2 + curr2 * dim3 + l * dim4]
              += memT[nn1 + 0 * dim2 + child2 * dim3 + 1 * dim4];
          }
        }
      }

      for(size_t i = 0; i < nn1; i++) {
        int curr1 = preorder1[i];
        std::vector<ftm::idNode> children1;
        tree1->getChildren(curr1, children1);
        for(size_t j = 0; j < nn2; j++) {
          int curr2 = preorder2[j];
          std::vector<ftm::idNode> children2;
          tree2->getChildren(curr2, children2);
          // compute optimal lookahead mapping between children
          bool hasSaddleChildren1 = false;
          bool hasSaddleChildren2 = false;
          for (auto c : children1){
            if(!tree1->isLeaf(c)){
              hasSaddleChildren1 = true;
              break;
            }
          }
          for (auto c : children2){
            if(!tree2->isLeaf(c)){
              hasSaddleChildren2 = true;
              break;
            }
          }
          tP1 = totalPersistence1[curr1];
          tP2 = totalPersistence2[curr2];
          dataType localLowerBound = tP1>tP2 ? tP1-tP2 : tP2-tP1;
          bool useLookahead = lookahead>0
                              and !children1.empty() and !children2.empty()
                              and hasSaddleChildren1 and hasSaddleChildren2
                              and localLowerBound<globalUpperBound;
          if(useLookahead){  
            std::stack<std::pair<ftm::idNode,ftm::idNode>> s;
            std::unordered_map<ftm::idNode,ftm::idNode> right;
            std::unordered_map<ftm::idNode,ftm::idNode> down;
            for (ftm::idNode ci=0; ci<children1.size()-1; ci++){
              s.push(std::make_pair(children1[ci],children1[ci+1]));
            }
            s.push(std::make_pair(children1.back(),children1.back()));
            while(not s.empty()){
              auto cn = s.top().first;
              auto cr = s.top().second;
              s.pop();
              if(predecessors1[cn].size()-predecessors1[curr1].size()>lookahead+1){
                continue;
              }
              right[cn] = cr;
              if(tree1->getNumberOfChildren(cn)>0){
                std::vector<unsigned int> children_cn;
                tree1->getChildren(cn, children_cn);
                down[cn] = children_cn.front();
                for(ftm::idNode ci=0; ci<children_cn.size()-1; ci++){
                  s.push(std::make_pair(children_cn[ci],children_cn[ci+1]));
                }
                s.push(std::make_pair(children_cn.back(),cn==cr?children_cn.back():cr));
              }
            }
            std::vector<std::pair<dataType,std::vector<unsigned int>>> cases1;
            std::vector<std::tuple<ftm::idNode,dataType,std::vector<unsigned int>>> worklist;
            cases1.reserve(right.size());
            worklist.reserve(right.size());
            worklist.push_back(std::make_tuple(children1[0],0,std::vector<unsigned int>()));
            while (!worklist.empty()){
              auto curr_tuple = worklist.back();
              auto cn = std::get<0>(curr_tuple);
              auto deleted_cost = std::get<1>(curr_tuple);
              auto kept_nodes = std::get<2>(curr_tuple);
              worklist.pop_back();
              auto p = predecessors1[cn].back();
              auto l = predecessors1[cn].size()-predecessors1[curr1].size();
              if (l<=lookahead and tree1->getNumberOfChildren(cn)>0){
                auto deletion_cost = editCost_Persistence<dataType>(cn,p,-1,-1,tree1,tree2);
                worklist.push_back(std::make_tuple(down[cn],deleted_cost+deletion_cost,kept_nodes));
              }
              std::vector<unsigned int> kept_nodes_ = kept_nodes;
              kept_nodes_.push_back(cn);
              if(right[cn]==cn){
                if(kept_nodes_.size()>1 && deleted_cost>0) cases1.push_back(std::make_pair(deleted_cost,kept_nodes_));
              }
              else{
                worklist.push_back(std::make_tuple(right[cn],deleted_cost,kept_nodes_));
              }
            }

            s = std::stack<std::pair<ftm::idNode,ftm::idNode>>();
            right = std::unordered_map<ftm::idNode,ftm::idNode>();
            down = std::unordered_map<ftm::idNode,ftm::idNode>();
            for (ftm::idNode ci=0; ci<children2.size()-1; ci++){
              s.push(std::make_pair(children2[ci],children2[ci+1]));
            }
            s.push(std::make_pair(children2.back(),children2.back()));
            while(not s.empty()){
              ftm::idNode cn = s.top().first;
              ftm::idNode cr = s.top().second;
              s.pop();
              if(predecessors2[cn].size()-predecessors2[curr2].size()>lookahead+1){
                continue;
              }
              right[cn] = cr;
              if(tree2->getNumberOfChildren(cn)>0){
                std::vector<unsigned int> children_cn;
                tree2->getChildren(cn, children_cn);
                down[cn] = children_cn.front();
                for(ftm::idNode ci=0; ci<children_cn.size()-1; ci++){
                  s.push(std::make_pair(children_cn[ci],children_cn[ci+1]));
                }
                s.push(std::make_pair(children_cn.back(),cn==cr?children_cn.back():cr));
              }
            }
            std::vector<std::pair<dataType,std::vector<unsigned int>>> cases2;
            worklist = std::vector<std::tuple<ftm::idNode,dataType,std::vector<unsigned int>>>();
            cases2.reserve(right.size());
            worklist.reserve(right.size());
            worklist.push_back(std::make_tuple(children2[0],0,std::vector<unsigned int>()));
            while (!worklist.empty()){
              auto curr_tuple = worklist.back();
              auto cn = std::get<0>(curr_tuple);
              auto deleted_cost = std::get<1>(curr_tuple);
              auto kept_nodes = std::get<2>(curr_tuple);
              worklist.pop_back();
              auto p = predecessors2[cn].back();
              auto l = predecessors2[cn].size()-predecessors2[curr2].size();
              if (l<=lookahead and tree2->getNumberOfChildren(cn)>0){
                auto deletion_cost = editCost_Persistence<dataType>(-1,-1,cn,p,tree1,tree2);
                worklist.push_back(std::make_tuple(down[cn],deleted_cost+deletion_cost,kept_nodes));
              }
              std::vector<unsigned int> kept_nodes_ = kept_nodes;
              kept_nodes_.push_back(cn);
              if(right[cn]==cn){
                if(kept_nodes_.size()>1 && deleted_cost>0) cases2.push_back(std::make_pair(deleted_cost,kept_nodes_));
              }
              else{
                worklist.push_back(std::make_tuple(right[cn],deleted_cost,kept_nodes_));
              }
            }

            dataType opt_case_cost = std::numeric_limits<dataType>::max();
            std::vector<std::pair<ftm::idNode,ftm::idNode>> opt_case;
            for (auto case1 : cases1){
              auto delete_costs1 = std::get<0>(case1);
              if(delete_costs1>globalUpperBound) continue;
              auto actual_children1 = std::get<1>(case1);
              dataType tP1_ = tP1-delete_costs1;
              dataType tP1__ = totalPersistence1[rootID1]-tP1;
              for (auto case2 : cases2){
                auto delete_costs2 = std::get<0>(case2);
                if(delete_costs1+delete_costs2>globalUpperBound) continue;
                dataType case_cost = delete_costs1+delete_costs2;
                dataType tP2_ = tP2-delete_costs2;
                dataType tP2__ = totalPersistence2[rootID2]-tP2;
                // dataType localBound = (tP1_>tP2_ ? tP1_-tP2_ : tP2_-tP1_) + case_cost;
                dataType localBound = (tP1__>tP2__ ? tP1__-tP2__ : tP2__-tP1__)
                                    + (tP1_>tP2_ ? tP1_-tP2_ : tP2_-tP1_) 
                                    + case_cost;
                if(localBound>globalUpperBound) continue;
                auto actual_children2 = std::get<1>(case2);

                auto f = [&](unsigned int r, unsigned int c) {
                  size_t const c1 = r < actual_children1.size()
                                      ? actual_children1[r]
                                      : nn1;
                  size_t const c2 = c < actual_children2.size()
                                      ? actual_children2[c]
                                      : nn2;
                  int const l1_ = c1 == nn1 ? 0 : 1;
                  int const l2_ = c2 == nn2 ? 0 : 1;
                  return memT[c1 + l1_ * dim2 + c2 * dim3 + l2_ * dim4];
                };
                int size = std::max(actual_children1.size(),actual_children2.size()) + 1;
                auto costMatrix = std::vector<std::vector<dataType>>(
                  size, std::vector<dataType>(size, 0));
                std::vector<MatchingType> matching;
                for(int r = 0; r < size; r++) {
                  for(int c = 0; c < size; c++) {
                    costMatrix[r][c] = f(r, c);
                  }
                }

                AssignmentSolver<dataType> *assignmentSolver;
                AssignmentExhaustive<dataType> solverExhaustive;
                AssignmentMunkres<dataType> solverMunkres;
                AssignmentAuction<dataType> solverAuction;
                switch(assignmentSolverID_) {
                  case 1:
                    solverExhaustive = AssignmentExhaustive<dataType>();
                    assignmentSolver = &solverExhaustive;
                    break;
                  case 2:
                    solverMunkres = AssignmentMunkres<dataType>();
                    assignmentSolver = &solverMunkres;
                    break;
                  case 0:
                  default:
                    solverAuction = AssignmentAuction<dataType>();
                    assignmentSolver = &solverAuction;
                }
                assignmentSolver->setInput(costMatrix);
                assignmentSolver->setBalanced(true);
                assignmentSolver->run(matching);
                for(auto m : matching)
                  case_cost += std::get<2>(m);
                opt_case_cost = std::min(opt_case_cost, case_cost);
                if(opt_case_cost==case_cost){
                  opt_case = std::vector<std::pair<ftm::idNode,ftm::idNode>>();
                  for(auto m : matching){
                    ftm::idNode m1 = std::get<0>(m);
                    ftm::idNode m2 = std::get<1>(m);
                    if(m1<actual_children1.size() and m2<actual_children2.size()){
                      opt_case.push_back(std::make_pair(actual_children1[m1],actual_children2[m2]));
                    }
                  }
                }
              }
            }

            memT[curr1 + 0 * dim2 + curr2 * dim3 + 0 * dim4] = opt_case_cost;
            memLA[curr1+curr2*nn1] = std::make_pair(opt_case_cost,opt_case);
          }
          // normal recursions
          for(size_t l1 = 1; l1 <= predecessors1[preorder1[i]].size(); l1++) {
            int parent1
              = predecessors1[preorder1[i]]
                             [predecessors1[preorder1[i]].size() - l1];
            for(size_t l2 = 1; l2 <= predecessors2[preorder2[j]].size(); l2++) {
              int parent2
                = predecessors2[preorder2[j]]
                               [predecessors2[preorder2[j]].size() - l2];

              //===============================================================================
              // If both trees not empty, find optimal edit operation

              //---------------------------------------------------------------------------
              // If both trees only have one branch, return edit cost between
              // the two branches
              if(tree1->getNumberOfChildren(curr1) == 0
                 and tree2->getNumberOfChildren(curr2) == 0) {
                memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4]
                  = editCost_Persistence<dataType>(
                    curr1, parent1, curr2, parent2, tree1, tree2);
              }
              //---------------------------------------------------------------------------
              // If first tree only has one branch, try all decompositions of
              // second tree
              else if(tree1->getNumberOfChildren(curr1) == 0) {
                dataType d = std::numeric_limits<dataType>::max();
                for(auto child2_mb : children2) {
                  dataType d_ = memT[curr1 + l1 * dim2 + child2_mb * dim3
                                     + (l2 + 1) * dim4];
                  for(auto child2 : children2) {
                    if(child2 == child2_mb) {
                      continue;
                    }
                    d_ += memT[nn1 + 0 * dim2 + child2 * dim3 + 1 * dim4];
                  }
                  d = std::min(d, d_);
                }
                memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] = d;
              }
              //---------------------------------------------------------------------------
              // If second tree only has one branch, try all decompositions of
              // first tree
              else if(tree2->getNumberOfChildren(curr2) == 0) {
                dataType d = std::numeric_limits<dataType>::max();
                for(auto child1_mb : children1) {
                  dataType d_ = memT[child1_mb + (l1 + 1) * dim2 + curr2 * dim3
                                     + l2 * dim4];
                  for(auto child1 : children1) {
                    if(child1 == child1_mb) {
                      continue;
                    }
                    d_ += memT[child1 + 1 * dim2 + nn2 * dim3 + 0 * dim4];
                  }
                  d = std::min(d, d_);
                }
                memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] = d;
              }
              //---------------------------------------------------------------------------
              // If both trees have more than one branch, try all decompositions
              // of both trees
              else {
                dataType d = std::numeric_limits<dataType>::max();
                //-----------------------------------------------------------------------
                // Try all possible main branches of first tree (child1_mb) and
                // all possible main branches of second tree (child2_mb) Then
                // try all possible matchings of subtrees
                if(tree1->getNumberOfChildren(curr1) == 2
                   && tree2->getNumberOfChildren(curr2) == 2) {
                  int const child11 = children1[0];
                  int const child12 = children1[1];
                  int const child21 = children2[0];
                  int const child22 = children2[1];
                  d = std::min<dataType>(
                    d, memT[child11 + 1 * dim2 + child21 * dim3 + 1 * dim4]
                         + memT[child12 + 1 * dim2 + child22 * dim3 + 1 * dim4]
                         + editCost_Persistence<dataType>(
                           curr1, parent1, curr2, parent2, tree1, tree2));
                  d = std::min<dataType>(
                    d, memT[child11 + 1 * dim2 + child22 * dim3 + 1 * dim4]
                         + memT[child12 + 1 * dim2 + child21 * dim3 + 1 * dim4]
                         + editCost_Persistence<dataType>(
                           curr1, parent1, curr2, parent2, tree1, tree2));
                } else {
                  auto f = [&](int r, int c) {
                    size_t const c1 = r < tree1->getNumberOfChildren(curr1)
                                        ? children1[r]
                                        : nn1;
                    size_t const c2 = c < tree2->getNumberOfChildren(curr2)
                                        ? children2[c]
                                        : nn2;
                    int const l1_ = c1 == nn1 ? 0 : 1;
                    int const l2_ = c2 == nn2 ? 0 : 1;
                    return memT[c1 + l1_ * dim2 + c2 * dim3 + l2_ * dim4];
                  };
                  int size = std::max(tree1->getNumberOfChildren(curr1),
                                      tree2->getNumberOfChildren(curr2))
                             + 1;
                  auto costMatrix = std::vector<std::vector<dataType>>(
                    size, std::vector<dataType>(size, 0));
                  std::vector<MatchingType> matching;
                  for(int r = 0; r < size; r++) {
                    for(int c = 0; c < size; c++) {
                      costMatrix[r][c] = f(r, c);
                    }
                  }

                  AssignmentSolver<dataType> *assignmentSolver;
                  AssignmentExhaustive<dataType> solverExhaustive;
                  AssignmentMunkres<dataType> solverMunkres;
                  AssignmentAuction<dataType> solverAuction;
                  switch(assignmentSolverID_) {
                    case 1:
                      solverExhaustive = AssignmentExhaustive<dataType>();
                      assignmentSolver = &solverExhaustive;
                      break;
                    case 2:
                      solverMunkres = AssignmentMunkres<dataType>();
                      assignmentSolver = &solverMunkres;
                      break;
                    case 0:
                    default:
                      solverAuction = AssignmentAuction<dataType>();
                      assignmentSolver = &solverAuction;
                  }
                  assignmentSolver->setInput(costMatrix);
                  assignmentSolver->setBalanced(true);
                  assignmentSolver->run(matching);
                  dataType d_ = editCost_Persistence<dataType>(
                    curr1, parent1, curr2, parent2, tree1, tree2);
                  for(auto m : matching)
                    d_ += std::get<2>(m);
                  d = std::min(d, d_);
                }
                
                //-----------------------------------------------------------------------
                // Try to look-ahead
                if(useLookahead){
                  dataType case_cost = editCost_Persistence<dataType>(curr1,parent1,curr2,parent2, tree1, tree2);
                  case_cost += memT[curr1 + 0 * dim2 + curr2 * dim3 + 0 * dim4];
                  d = std::min(d, case_cost);
                }
                //-----------------------------------------------------------------------
                // Try to continue main branch on one child of first tree and
                // delete all other subtrees Then match continued branch to
                // current branch in second tree
                for(auto child1_mb : children1) {
                  dataType d_ = memT[child1_mb + (l1 + 1) * dim2 + curr2 * dim3
                                     + l2 * dim4];
                  for(auto child1 : children1) {
                    if(child1 == child1_mb) {
                      continue;
                    }
                    d_ += memT[child1 + 1 * dim2 + nn2 * dim3 + 0 * dim4];
                  }
                  d = std::min(d, d_);
                }
                //-----------------------------------------------------------------------
                // Try to continue main branch on one child of second tree and
                // delete all other subtrees Then match continued branch to
                // current branch in first tree
                for(auto child2_mb : children2) {
                  dataType d_ = memT[curr1 + l1 * dim2 + child2_mb * dim3
                                     + (l2 + 1) * dim4];
                  for(auto child2 : children2) {
                    if(child2 == child2_mb) {
                      continue;
                    }
                    d_ += memT[nn1 + 0 * dim2 + child2 * dim3 + 1 * dim4];
                  }
                  d = std::min(d, d_);
                }
                memT[curr1 + l1 * dim2 + curr2 * dim3 + l2 * dim4] = d;
              }
            }
          }
        }
      }

      std::vector<ftm::idNode> children1;
      tree1->getChildren(rootID1, children1);
      std::vector<ftm::idNode> children2;
      tree2->getChildren(rootID2, children2);

      dataType res
        = memT[children1[0] + 1 * dim2 + children2[0] * dim3 + 1 * dim4];

      if(computeMapping_ && outputMatching) {

        outputMatching->clear();
        traceMapping_path(tree1, tree2, children1[0], 1, children2[0], 1,
                          predecessors1, predecessors2, depth1, depth2, memT, memLA,
                          *outputMatching);

        // dataType cost_mapping = 0;
        // dataType cost_ins = 0;
        // dataType cost_del = 0;
        // std::vector<bool> matchedNodes1(tree1->getNumberOfNodes(),false);
        // std::vector<bool> matchedNodes2(tree2->getNumberOfNodes(),false);
        // for(auto m : *outputMatching){
        //   matchedNodes1[m.first.first] = true;
        //   matchedNodes1[m.first.second] = true;
        //   matchedNodes2[m.second.first] = true;
        //   matchedNodes2[m.second.second] = true;
        //   ftm::idNode cn = tree1->getParentSafe(m.first.first);
        //   while(cn!=m.first.second){
        //     matchedNodes1[cn] = true;
        //     cn = tree1->getParentSafe(cn);
        //   }
        //   cn = tree2->getParentSafe(m.second.first);
        //   while(cn!=m.second.second){
        //     matchedNodes2[cn] = true;
        //     cn = tree2->getParentSafe(cn);
        //   }
        //   dataType cost = editCost_Persistence<dataType>(
        //                     m.first.first, m.first.second, m.second.first,
        //                     m.second.second, tree1, tree2);
        //   cost_mapping += cost;
        //   // std::cout << "   (" << m.first.first << " " << m.first.second <<
        //   ") - (" << m.second.first << " " << m.second.second << ") : " <<
        //   cost << "\n";
        // }
        // for(ftm::idNode i=0; i<tree1->getNumberOfNodes(); i++){
        //   if(!matchedNodes1[i]){
        //     dataType cost = editCost_Persistence<dataType>(i,
        //     tree1->getParentSafe(i), -1, -1, tree1, tree2);
        //     // std::cout << "   (" << i << " " << tree1->getParentSafe(i) <<
        //     ") - (" << -1 << " " << -1 << ") : " << cost << "\n"; cost_del +=
        //     cost;
        //   }
        // }
        // for(ftm::idNode i=0; i<tree2->getNumberOfNodes(); i++){
        //   if(!matchedNodes2[i]){
        //     dataType cost = editCost_Persistence<dataType>(-1, -1, i,
        //     tree2->getParentSafe(i), tree1, tree2);
        //     // std::cout << "   (" << -1 << " " << -1 << ") - (" << i << " "
        //     << tree2->getParentSafe(i) << ") : " << cost << "\n"; cost_ins +=
        //     cost;
        //   }
        // }
        // // std::cout << res << " " << cost_mapping+cost_ins+cost_del <<
        // std::endl;
        // // std::cout << res << " " << cost_mapping << std::endl;
      }

      return squared_ ? std::sqrt(res) : res;
    }

    template <class dataType>
    dataType execute(ftm::MergeTree<dataType> &mTree1,
                     ftm::MergeTree<dataType> &mTree2,
                     std::vector<std::pair<std::pair<ftm::idNode, ftm::idNode>,
                                           std::pair<ftm::idNode, ftm::idNode>>>
                       *outputMatching) {

      ftm::MergeTree<dataType> mTree1Copy;
      ftm::MergeTree<dataType> mTree2Copy;
      if(saveTree_) {
        mTree1Copy = ftm::copyMergeTree<dataType>(mTree1);
        mTree2Copy = ftm::copyMergeTree<dataType>(mTree2);
      }
      ftm::MergeTree<dataType> &mTree1Int = (saveTree_ ? mTree1Copy : mTree1);
      ftm::MergeTree<dataType> &mTree2Int = (saveTree_ ? mTree2Copy : mTree2);
      ftm::FTMTree_MT *tree1 = &(mTree1Int.tree);
      ftm::FTMTree_MT *tree2 = &(mTree2Int.tree);

      // optional preprocessing
      if(preprocess_) {
        treesNodeCorr_.resize(2);
        preprocessingPipeline<dataType>(
          mTree1Int, epsilonTree1_, epsilon2Tree1_, epsilon3Tree1_,
          branchDecomposition_, useMinMaxPair_, cleanTree_, treesNodeCorr_[0],
          true, true);
        preprocessingPipeline<dataType>(
          mTree2Int, epsilonTree2_, epsilon2Tree2_, epsilon3Tree2_,
          branchDecomposition_, useMinMaxPair_, cleanTree_, treesNodeCorr_[1],
          true, true);
      }

      tree1 = &(mTree1Int.tree);
      tree2 = &(mTree2Int.tree);

      return computeDistance<dataType>(tree1, tree2, outputMatching);
    }

    template <class dataType>
    dataType
      computeDistance(ftm::FTMTree_MT *tree1,
                      ftm::FTMTree_MT *tree2,
                      std::vector<std::tuple<ftm::idNode, ftm::idNode, double>>
                        *outputMatching) {

      std::vector<std::vector<ftm::idNode>> children1(tree1->getNumberOfNodes());
      std::vector<ftm::idNode> parents1(tree1->getNumberOfNodes());
      for(ftm::idNode nIdx=0; nIdx<tree1->getNumberOfNodes(); nIdx++){
        tree1->getChildren(nIdx,children1[nIdx]);
        parents1[nIdx] = tree1->getParentSafe(nIdx);
      }
      std::vector<std::vector<ftm::idNode>> children2(tree2->getNumberOfNodes());
      std::vector<ftm::idNode> parents2(tree2->getNumberOfNodes());
      for(ftm::idNode nIdx=0; nIdx<tree2->getNumberOfNodes(); nIdx++){
        tree2->getChildren(nIdx,children2[nIdx]);
        parents2[nIdx] = tree2->getParentSafe(nIdx);
      }

      std::vector<int> matchedNodes(tree1->getNumberOfNodes(), -1);
      std::vector<std::pair<std::pair<ftm::idNode, ftm::idNode>,
                            std::pair<ftm::idNode, ftm::idNode>>>
        mapping;
      dataType res = computeDistance<dataType>(tree1, tree2, &mapping);
      if(computeMapping_ && outputMatching) {
        outputMatching->clear();
        for(auto m : mapping) {
          matchedNodes[m.first.first] = m.second.first;
          if(m.first.second==tree1->getRoot())
            matchedNodes[m.first.second] = m.second.second;
        }
        for(ftm::idNode i = 0; i < matchedNodes.size(); i++) {
          if(matchedNodes[i] >= 0)
            outputMatching->emplace_back(
              std::make_tuple(i, matchedNodes[i], 0.0));
        }
      }

      return res;
    }

    template <class dataType>
    dataType execute(ftm::MergeTree<dataType> &mTree1,
                     ftm::MergeTree<dataType> &mTree2,
                     std::vector<std::tuple<ftm::idNode, ftm::idNode, double>>
                       *outputMatching) {

      ftm::MergeTree<dataType> mTree1Copy;
      ftm::MergeTree<dataType> mTree2Copy;
      if(saveTree_) {
        mTree1Copy = ftm::copyMergeTree<dataType>(mTree1);
        mTree2Copy = ftm::copyMergeTree<dataType>(mTree2);
      }
      ftm::MergeTree<dataType> &mTree1Int = (saveTree_ ? mTree1Copy : mTree1);
      ftm::MergeTree<dataType> &mTree2Int = (saveTree_ ? mTree2Copy : mTree2);
      ftm::FTMTree_MT *tree1 = &(mTree1Int.tree);
      ftm::FTMTree_MT *tree2 = &(mTree2Int.tree);

      // optional preprocessing
      if(preprocess_) {
        treesNodeCorr_.resize(2);
        preprocessingPipeline<dataType>(
          mTree1Int, epsilonTree1_, epsilon2Tree1_, epsilon3Tree1_, false,
          useMinMaxPair_, cleanTree_, treesNodeCorr_[0], true, true);
        preprocessingPipeline<dataType>(
          mTree2Int, epsilonTree2_, epsilon2Tree2_, epsilon3Tree2_, false,
          useMinMaxPair_, cleanTree_, treesNodeCorr_[1], true, true);
      }

      tree1 = &(mTree1Int.tree);
      tree2 = &(mTree2Int.tree);

      return computeDistance<dataType>(tree1, tree2, outputMatching);
    }

    template <class dataType>
    dataType computeDistance(ftm::FTMTree_MT *tree1, ftm::FTMTree_MT *tree2) {
      return computeDistance<dataType>(
        tree1, tree2,
        (std::vector<std::pair<std::pair<ftm::idNode, ftm::idNode>,
                               std::pair<ftm::idNode, ftm::idNode>>> *)nullptr);
    }

    template <class dataType>
    dataType execute(ftm::MergeTree<dataType> &mTree1,
                     ftm::MergeTree<dataType> &mTree2) {

      ftm::MergeTree<dataType> mTree1Copy;
      ftm::MergeTree<dataType> mTree2Copy;
      if(saveTree_) {
        mTree1Copy = ftm::copyMergeTree<dataType>(mTree1);
        mTree2Copy = ftm::copyMergeTree<dataType>(mTree2);
      }
      ftm::MergeTree<dataType> &mTree1Int = (saveTree_ ? mTree1Copy : mTree1);
      ftm::MergeTree<dataType> &mTree2Int = (saveTree_ ? mTree2Copy : mTree2);
      ftm::FTMTree_MT *tree1 = &(mTree1Int.tree);
      ftm::FTMTree_MT *tree2 = &(mTree2Int.tree);

      // optional preprocessing
      if(preprocess_) {
        treesNodeCorr_.resize(2);
        preprocessingPipeline<dataType>(
          mTree1Int, epsilonTree1_, epsilon2Tree1_, epsilon3Tree1_, false,
          useMinMaxPair_, cleanTree_, treesNodeCorr_[0], true, true);
        preprocessingPipeline<dataType>(
          mTree2Int, epsilonTree2_, epsilon2Tree2_, epsilon3Tree2_, false,
          useMinMaxPair_, cleanTree_, treesNodeCorr_[1], true, true);
      }

      tree1 = &(mTree1Int.tree);
      tree2 = &(mTree2Int.tree);

      return computeDistance<dataType>(
        tree1, tree2,
        (std::vector<std::pair<std::pair<ftm::idNode, ftm::idNode>,
                               std::pair<ftm::idNode, ftm::idNode>>> *)nullptr);
    }
  };
} // namespace ttk
