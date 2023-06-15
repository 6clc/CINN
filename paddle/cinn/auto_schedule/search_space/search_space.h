// Copyright (c) 2022 CINN Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "cinn/auto_schedule/cost_model/expr_cost_model.h"
#include "cinn/auto_schedule/search_space/auto_gen_rule/auto_gen_rule.h"
#include "cinn/auto_schedule/search_space/rule_sampler.h"
#include "cinn/auto_schedule/search_space/search_state.h"
#include "cinn/auto_schedule/task/tune_task.h"
#include "cinn/ir/ir_base.h"
#include "cinn/ir/ir_schedule.h"

namespace cinn {
namespace auto_schedule {

/**
 * This class is an abstraction of the transformations can be applied to
 * ir::Expr during auto-tuning. The transformation can be:
 *
 * 1. Manual defined schedule
 * 2. Schedule generated by AutoGenRule
 *
 * TODO(zhhsplendid): de-duplication the generated ModuleExpr
 */
class SearchSpace {
 public:
  SearchSpace(const TuneTask& tune_task, utils::LinearRandomEngine::StateType rand_seed = -1);

  // Sketch mutate, returns the mutated ModuleExpr and estimited cost
  virtual SearchState GetScheduleMutate(const SearchState& state, const ExprCostModel& cost_model);

  /**
   * \brief Generate sketch as initial population of evolutionary search.
   * @param num The number of sketches to generate.
   * @param strategy The strategy to generate sketchs,
   *        Current optional strategies are "rule_prune" or "random_prune" or "random".
   * - "rule_prune": will use rules to prune and generate sketches as efficiently as possible.
   * - "random_prune": will use the new interface ApplySketchRules() to simulate the random generation of sketches,
   *    and supports the function of a rule returning multiple SearchStates and random pruning by probability.
   * - "random": will randomly select a block and a rule to apply and repeat this step several times,
   *    however, each rule can only be used on one SearchState at most once.
   * @return  Generated sketchs.
   */
  virtual std::vector<SearchState> GenerateSketches(int num, const std::string& strategy);

 private:
  // TODO(zhhsplendid): mutate by manual schedule.
  SearchState ManualScheduleMutate(const SearchState& state);

  // mutate by sketch rules randomly
  SearchState RandomScheduleMutate(const SearchState& state);

  // Generate num sketchs, each with several rounds of SketchMutate
  std::vector<SearchState> InitSketchWithRandomStrategy(int num);

  // Generate sketch pruned randomly as initial population of evolutionary search
  std::vector<SearchState> InitSketchWithRandomPrunedStrategy();

  // Generate sketch pruned by rules as initial population of evolutionary search
  std::vector<SearchState> InitSketchWithRulePrunedStrategy();

  /**
   * @brief Collect the new states that may be transferred to after applying several rules on a block from a certain
   * state.
   * @param state Starting point of state transition.
   * @param block_name Name of the block to apply the rules to.
   * @param rule_sampler Sampler that samples the new rule to apply on the block.
   * @param steps Number of steps to apply the rule.
   * @param prune_by_rule If true, prune the state transition tree by rule, otherwise prune randomly.
   * @param prune_probability Pruning probability of random pruning.
   */
  std::vector<SearchState> ApplySketchRule(const SearchState& state,
                                           const std::string& block_name,
                                           RuleSampler* rule_sampler,
                                           int steps,
                                           bool prune_by_rule,
                                           double prune_probability = 1);

 private:
  const TuneTask& tune_task_;
  int init_sketch_random_depth_ = 6;
  // supported AutoGenRules, every task holds a set
  std::vector<std::unique_ptr<AutoGenRule>> sketch_rules_;
  utils::LinearRandomEngine::StateType rand_seed_;
};

}  // namespace auto_schedule
}  // namespace cinn
