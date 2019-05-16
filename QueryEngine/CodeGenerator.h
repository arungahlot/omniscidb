/*
 * Copyright 2019 OmniSci, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <llvm/IR/Value.h>

#include "../Analyzer/Analyzer.h"
#include "Execute.h"

class CodeGenerator {
 public:
  CodeGenerator(Executor::CgenState* cgen_state, Executor* executor)
      : cgen_state_(cgen_state), executor_(executor) {}

  std::vector<llvm::Value*> codegen(const Analyzer::Expr*,
                                    const bool fetch_columns,
                                    const CompilationOptions&);

  std::vector<llvm::Value*> codegen(const Analyzer::ColumnVar*,
                                    const bool fetch_column,
                                    const CompilationOptions&);

  std::vector<llvm::Value*> codegenHoistedConstants(
      const std::vector<const Analyzer::Constant*>& constants,
      const EncodingType enc_type,
      const int dict_id);

  llvm::ConstantInt* codegenIntConst(const Analyzer::Constant* constant);

  llvm::Value* codegenCastBetweenIntTypes(llvm::Value* operand_lv,
                                          const SQLTypeInfo& operand_ti,
                                          const SQLTypeInfo& ti,
                                          bool upscale = true);

  llvm::Value* posArg(const Analyzer::Expr*) const;

  llvm::Value* toBool(llvm::Value*);

  llvm::Value* castArrayPointer(llvm::Value* ptr, const SQLTypeInfo& elem_ti);

  static bool prioritizeQuals(const RelAlgExecutionUnit& ra_exe_unit,
                              std::vector<Analyzer::Expr*>& primary_quals,
                              std::vector<Analyzer::Expr*>& deferred_quals);

 private:
  std::vector<llvm::Value*> codegen(const Analyzer::Constant*,
                                    const EncodingType enc_type,
                                    const int dict_id,
                                    const CompilationOptions&);

  llvm::Value* codegenArith(const Analyzer::BinOper*, const CompilationOptions&);

  llvm::Value* codegenUMinus(const Analyzer::UOper*, const CompilationOptions&);

  llvm::Value* codegenCmp(const Analyzer::BinOper*, const CompilationOptions&);

  llvm::Value* codegenCmp(const SQLOps,
                          const SQLQualifier,
                          std::vector<llvm::Value*>,
                          const SQLTypeInfo&,
                          const Analyzer::Expr*,
                          const CompilationOptions&);

  llvm::Value* codegenIsNull(const Analyzer::UOper*, const CompilationOptions&);

  llvm::Value* codegenIsNullNumber(llvm::Value*, const SQLTypeInfo&);

  llvm::Value* codegenLogical(const Analyzer::BinOper*, const CompilationOptions&);

  llvm::Value* codegenLogical(const Analyzer::UOper*, const CompilationOptions&);

  llvm::Value* codegenCast(const Analyzer::UOper*, const CompilationOptions&);

  llvm::Value* codegenCast(llvm::Value* operand_lv,
                           const SQLTypeInfo& operand_ti,
                           const SQLTypeInfo& ti,
                           const bool operand_is_const,
                           const CompilationOptions& co);

  llvm::Value* codegen(const Analyzer::InValues*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::InIntegerSet* expr, const CompilationOptions& co);

  std::vector<llvm::Value*> codegen(const Analyzer::CaseExpr*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::ExtractExpr*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::DateaddExpr*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::DatediffExpr*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::DatetruncExpr*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::CharLengthExpr*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::KeyForStringExpr*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::LikeExpr*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::RegexpExpr*, const CompilationOptions&);

  llvm::Value* codegenUnnest(const Analyzer::UOper*, const CompilationOptions&);

  llvm::Value* codegenArrayAt(const Analyzer::BinOper*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::CardinalityExpr*, const CompilationOptions&);

  std::vector<llvm::Value*> codegenArrayExpr(const Analyzer::ArrayExpr*,
                                             const CompilationOptions&);

  llvm::Value* codegenFunctionOper(const Analyzer::FunctionOper*,
                                   const CompilationOptions&);

  llvm::Value* codegenFunctionOperWithCustomTypeHandling(
      const Analyzer::FunctionOperWithCustomTypeHandling*,
      const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::BinOper*, const CompilationOptions&);

  llvm::Value* codegen(const Analyzer::UOper*, const CompilationOptions&);

  std::vector<llvm::Value*> codegenHoistedConstantsLoads(const SQLTypeInfo& type_info,
                                                         const EncodingType enc_type,
                                                         const int dict_id,
                                                         const int16_t lit_off);

  std::vector<llvm::Value*> codegenHoistedConstantsPlaceholders(
      const SQLTypeInfo& type_info,
      const EncodingType enc_type,
      const int16_t lit_off,
      const std::vector<llvm::Value*>& literal_loads);

  std::vector<llvm::Value*> codegenColVar(const Analyzer::ColumnVar*,
                                          const bool fetch_column,
                                          const bool update_query_plan,
                                          const CompilationOptions&);

  llvm::Value* codegenFixedLengthColVar(const Analyzer::ColumnVar* col_var,
                                        llvm::Value* col_byte_stream,
                                        llvm::Value* pos_arg);

  // Generates code for a fixed length column when a window function is active.
  llvm::Value* codegenFixedLengthColVarInWindow(const Analyzer::ColumnVar* col_var,
                                                llvm::Value* col_byte_stream,
                                                llvm::Value* pos_arg);

  // Generate the position for the given window function and the query iteration position.
  llvm::Value* codegenWindowPosition(WindowFunctionContext* window_func_context,
                                     llvm::Value* pos_arg);

  std::vector<llvm::Value*> codegenVariableLengthStringColVar(
      llvm::Value* col_byte_stream,
      llvm::Value* pos_arg);

  llvm::Value* codegenRowId(const Analyzer::ColumnVar* col_var,
                            const CompilationOptions& co);

  llvm::Value* codgenAdjustFixedEncNull(llvm::Value*, const SQLTypeInfo&);

  std::vector<llvm::Value*> codegenOuterJoinNullPlaceholder(
      const Analyzer::ColumnVar* col_var,
      const bool fetch_column,
      const CompilationOptions& co);

  llvm::Value* codegenIntArith(const Analyzer::BinOper*, llvm::Value*, llvm::Value*);

  llvm::Value* codegenFpArith(const Analyzer::BinOper*, llvm::Value*, llvm::Value*);

  llvm::Value* codegenCastTimestampToDate(llvm::Value* ts_lv,
                                          const int dimen,
                                          const bool nullable);

  llvm::Value* codegenCastBetweenTimestamps(llvm::Value* ts_lv,
                                            const int operand_dimen,
                                            const int target_dimen,
                                            const bool nullable);

  llvm::Value* codegenCastFromString(llvm::Value* operand_lv,
                                     const SQLTypeInfo& operand_ti,
                                     const SQLTypeInfo& ti,
                                     const bool operand_is_const,
                                     const CompilationOptions& co);

  llvm::Value* codegenCastToFp(llvm::Value* operand_lv,
                               const SQLTypeInfo& operand_ti,
                               const SQLTypeInfo& ti);

  llvm::Value* codegenCastFromFp(llvm::Value* operand_lv,
                                 const SQLTypeInfo& operand_ti,
                                 const SQLTypeInfo& ti);

  llvm::Value* codegenAdd(const Analyzer::BinOper*,
                          llvm::Value*,
                          llvm::Value*,
                          const std::string& null_typename,
                          const std::string& null_check_suffix,
                          const SQLTypeInfo&);

  llvm::Value* codegenSub(const Analyzer::BinOper*,
                          llvm::Value*,
                          llvm::Value*,
                          const std::string& null_typename,
                          const std::string& null_check_suffix,
                          const SQLTypeInfo&);

  void codegenSkipOverflowCheckForNull(llvm::Value* lhs_lv,
                                       llvm::Value* rhs_lv,
                                       llvm::BasicBlock* no_overflow_bb,
                                       const SQLTypeInfo& ti);

  llvm::Value* codegenMul(const Analyzer::BinOper*,
                          llvm::Value*,
                          llvm::Value*,
                          const std::string& null_typename,
                          const std::string& null_check_suffix,
                          const SQLTypeInfo&,
                          bool downscale = true);

  llvm::Value* codegenDiv(llvm::Value*,
                          llvm::Value*,
                          const std::string& null_typename,
                          const std::string& null_check_suffix,
                          const SQLTypeInfo&,
                          bool upscale = true);

  llvm::Value* codegenDeciDiv(const Analyzer::BinOper*, const CompilationOptions&);

  llvm::Value* codegenMod(llvm::Value*,
                          llvm::Value*,
                          const std::string& null_typename,
                          const std::string& null_check_suffix,
                          const SQLTypeInfo&);

  llvm::Value* codegenCase(const Analyzer::CaseExpr*,
                           llvm::Type* case_llvm_type,
                           const bool is_real_str,
                           const CompilationOptions&);

  llvm::Value* codegenExtractHighPrecisionTimestamps(llvm::Value*,
                                                     const SQLTypeInfo&,
                                                     const ExtractField&);

  llvm::Value* codegenDateTruncHighPrecisionTimestamps(llvm::Value*,
                                                       const SQLTypeInfo&,
                                                       const DatetruncField&);

  llvm::Value* codegenCmpDecimalConst(const SQLOps,
                                      const SQLQualifier,
                                      const Analyzer::Expr*,
                                      const SQLTypeInfo&,
                                      const Analyzer::Expr*,
                                      const CompilationOptions&);

  llvm::Value* codegenOverlaps(const SQLOps,
                               const SQLQualifier,
                               const std::shared_ptr<Analyzer::Expr>,
                               const std::shared_ptr<Analyzer::Expr>,
                               const CompilationOptions&);

  llvm::Value* codegenStrCmp(const SQLOps,
                             const SQLQualifier,
                             const std::shared_ptr<Analyzer::Expr>,
                             const std::shared_ptr<Analyzer::Expr>,
                             const CompilationOptions&);

  llvm::Value* codegenQualifierCmp(const SQLOps,
                                   const SQLQualifier,
                                   std::vector<llvm::Value*>,
                                   const Analyzer::Expr*,
                                   const CompilationOptions&);

  llvm::Value* codegenLogicalShortCircuit(const Analyzer::BinOper*,
                                          const CompilationOptions&);

  llvm::Value* codegenDictLike(const std::shared_ptr<Analyzer::Expr> arg,
                               const Analyzer::Constant* pattern,
                               const bool ilike,
                               const bool is_simple,
                               const char escape_char,
                               const CompilationOptions&);

  llvm::Value* codegenDictStrCmp(const std::shared_ptr<Analyzer::Expr>,
                                 const std::shared_ptr<Analyzer::Expr>,
                                 const SQLOps,
                                 const CompilationOptions& co);

  llvm::Value* codegenDictRegexp(const std::shared_ptr<Analyzer::Expr> arg,
                                 const Analyzer::Constant* pattern,
                                 const char escape_char,
                                 const CompilationOptions&);

  // Returns the IR value which holds true iff at least one match has been found for outer
  // join, null if there's no outer join condition on the given nesting level.
  llvm::Value* foundOuterJoinMatch(const ssize_t nesting_level) const;

  llvm::Value* resolveGroupedColumnReference(const Analyzer::ColumnVar*);

  llvm::Value* colByteStream(const Analyzer::ColumnVar* col_var,
                             const bool fetch_column,
                             const bool hoist_literals);

  std::shared_ptr<const Analyzer::Expr> hashJoinLhs(const Analyzer::ColumnVar* rhs) const;

  std::shared_ptr<const Analyzer::ColumnVar> hashJoinLhsTuple(
      const Analyzer::ColumnVar* rhs,
      const Analyzer::BinOper* tautological_eq) const;

  std::unique_ptr<InValuesBitmap> createInValuesBitmap(const Analyzer::InValues*,
                                                       const CompilationOptions&);

  bool checkExpressionRanges(const Analyzer::UOper*, int64_t, int64_t);

  bool checkExpressionRanges(const Analyzer::BinOper*, int64_t, int64_t);

  struct ArgNullcheckBBs {
    llvm::BasicBlock* args_null_bb;
    llvm::BasicBlock* args_notnull_bb;
    llvm::BasicBlock* orig_bb;
  };

  ArgNullcheckBBs beginArgsNullcheck(const Analyzer::FunctionOper* function_oper,
                                     const std::vector<llvm::Value*>& orig_arg_lvs);

  llvm::Value* endArgsNullcheck(const ArgNullcheckBBs&,
                                llvm::Value*,
                                const Analyzer::FunctionOper*);

  llvm::Value* codegenFunctionOperNullArg(const Analyzer::FunctionOper*,
                                          const std::vector<llvm::Value*>&);

  std::vector<llvm::Value*> codegenFunctionOperCastArgs(
      const Analyzer::FunctionOper*,
      const ExtensionFunction*,
      const std::vector<llvm::Value*>&,
      const std::unordered_map<llvm::Value*, llvm::Value*>&,
      const CompilationOptions&);

  Executor::CgenState* cgen_state_;
  Executor* executor_;
};