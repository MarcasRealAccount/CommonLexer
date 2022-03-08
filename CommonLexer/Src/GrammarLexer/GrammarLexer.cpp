#include "GrammarLexer/GrammarLexer.h"
#include "CommonLexer/Matchers.h"
#include "CommonLexer/Rules.h"

namespace GrammarLexer
{
	GrammarLexer::GrammarLexer()
	{
		using namespace CommonLexer;

		setMainRule("File");

		registerRule(MatcherRule {
		    "File",
		    RangeMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Declaration"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace)),
		    false });
		registerRule(MatcherRule {
		    "Declaration",
		    OrMatcher(
		        ReferenceMatcher("EmptyDeclaration"),
		        ReferenceMatcher("BlockDeclaration"),
		        ReferenceMatcher("OptionDeclaration"),
		        ReferenceMatcher("RuleDeclaration"),
		        ReferenceMatcher("NodelessRuleDeclaration"),
		        ReferenceMatcher("CallbackRuleDeclaration"),
		        ReferenceMatcher("Comment")),
		    false });
		registerRule(MatcherRule { "EmptyDeclaration", TextMatcher(";"), false });
		registerRule(MatcherRule {
		    "BlockDeclaration",
		    CombinationMatcher(
		        SpaceMatcher(
		            TextMatcher("{"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        RangeMatcher(
		            SpaceMatcher(
		                ReferenceMatcher("Declaration"),
		                false,
		                ESpaceDirection::Both,
		                ESpaceMethod::Whitespace)),
		        TextMatcher("}")) });
		registerRule(MatcherRule {
		    "OptionDeclaration",
		    CombinationMatcher(
		        SpaceMatcher(
		            TextMatcher("!"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Identifier"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher("="),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Value"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher(";")) });
		registerRule(MatcherRule {
		    "RuleDeclaration",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Identifier"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher(":"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Depth1Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher(";")) });
		registerRule(MatcherRule {
		    "NodelessRuleDeclaration",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Identifier"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher("?"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher(":"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Depth1Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher(";")) });
		registerRule(MatcherRule {
		    "CallbackRuleDeclaration",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Identifier"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher("!"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher(";"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace)) });
		registerRule(MatcherRule { "Comment", RegexMatcher("#.*"), false });

		registerRule(MatcherRule { "Identifier", RegexMatcher("[A-Za-z_][A-Za-z0-9_]*") });

		registerRule(MatcherRule {
		    "Value",
		    OrMatcher(
		        ReferenceMatcher("Identifier"),
		        ReferenceMatcher("Boolean"),
		        ReferenceMatcher("Integer"),
		        ReferenceMatcher("TextMatcher")),
		    false });
		registerRule(MatcherRule {
		    "Boolean",
		    OrMatcher(
		        TextMatcher("true"),
		        TextMatcher("false")) });
		registerRule(MatcherRule { "Integer", RegexMatcher("[0-9]+") });

		registerRule(MatcherRule {
		    "Depth1Matcher",
		    OrMatcher(
		        ReferenceMatcher("Branch"),
		        ReferenceMatcher("Depth2Matcher")),
		    false });
		registerRule(MatcherRule {
		    "Depth2Matcher",
		    OrMatcher(
		        ReferenceMatcher("CombinationMatcher"),
		        ReferenceMatcher("OrMatcher"),
		        ReferenceMatcher("Depth3Matcher")),
		    false });
		registerRule(MatcherRule {
		    "Depth3Matcher",
		    OrMatcher(
		        ReferenceMatcher("ZeroOrMore"),
		        ReferenceMatcher("OneOrMore"),
		        ReferenceMatcher("ExactAmount"),
		        ReferenceMatcher("RangeMatcher"),
		        ReferenceMatcher("OptionalMatcher"),
		        ReferenceMatcher("NegativeMatcher"),
		        ReferenceMatcher("Depth4Matcher")),
		    false });
		registerRule(MatcherRule {
		    "Depth4Matcher",
		    OrMatcher(
		        ReferenceMatcher("LenientSpaceMatcher"),
		        ReferenceMatcher("ForcedSpaceMatcher"),
		        ReferenceMatcher("Depth5Matcher")),
		    false });
		registerRule(MatcherRule {
		    "Depth5Matcher",
		    OrMatcher(
		        ReferenceMatcher("Group"),
		        ReferenceMatcher("NamedGroupMatcher"),
		        ReferenceMatcher("NamedGroupReferenceMatcher"),
		        ReferenceMatcher("ReferenceMatcher"),
		        ReferenceMatcher("TextMatcher"),
		        ReferenceMatcher("RegexMatcher")),
		    false });

		registerRule(MatcherRule {
		    "Branch",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth2Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        RangeMatcher(
		            SpaceMatcher(
		                CombinationMatcher(
		                    SpaceMatcher(
		                        TextMatcher(";"),
		                        false,
		                        ESpaceDirection::Both,
		                        ESpaceMethod::Whitespace),
		                    ReferenceMatcher("Depth2Matcher")),
		                false,
		                ESpaceDirection::Both,
		                ESpaceMethod::Whitespace),
		            1)) });

		registerRule(MatcherRule {
		    "CombinationMatcher",
		    CombinationMatcher(
		        ReferenceMatcher("Depth3Matcher"),
		        RangeMatcher(
		            SpaceMatcher(
		                ReferenceMatcher("Depth3Matcher"),
		                true,
		                ESpaceDirection::Left,
		                ESpaceMethod::Whitespace),
		            1)) });
		registerRule(MatcherRule {
		    "OrMatcher",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth3Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        RangeMatcher(
		            SpaceMatcher(
		                CombinationMatcher(Tuple {
		                    SpaceMatcher(
		                        TextMatcher("|"),
		                        false,
		                        ESpaceDirection::Both,
		                        ESpaceMethod::Whitespace),
		                    ReferenceMatcher("Depth3Matcher") }),
		                false,
		                ESpaceDirection::Both,
		                ESpaceMethod::Whitespace),
		            1)) });

		registerRule(MatcherRule {
		    "ZeroOrMore",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth4Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher("*")) });
		registerRule(MatcherRule {
		    "OneOrMore",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth4Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher("+")) });
		registerRule(MatcherRule {
		    "ExactAmount",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth4Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher("{"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Integer"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher("}")) });
		registerRule(MatcherRule {
		    "RangeMatcher",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth4Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher("{"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Integer"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher(","),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Integer"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher("}")) });
		registerRule(MatcherRule {
		    "OptionalMatcher",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth4Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher("?"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        NegativeMatcher(TextMatcher(":"))) });
		registerRule(MatcherRule {
		    "NegativeMatcher",
		    CombinationMatcher(
		        SpaceMatcher(
		            TextMatcher("~"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        ReferenceMatcher("Depth4Matcher")) });

		registerRule(MatcherRule {
		    "LenientSpaceMatcher",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth5Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher(",")) });
		registerRule(MatcherRule {
		    "ForcedSpaceMatcher",
		    CombinationMatcher(
		        SpaceMatcher(
		            ReferenceMatcher("Depth5Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher(".")) });

		registerRule(MatcherRule {
		    "Group",
		    CombinationMatcher(
		        SpaceMatcher(
		            TextMatcher("("),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Depth2Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher(")")) });
		registerRule(MatcherRule {
		    "NamedGroupMatcher",
		    CombinationMatcher(
		        SpaceMatcher(
		            TextMatcher("("),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher("<"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Identifier"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher(">"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            TextMatcher(":"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Depth2Matcher"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        TextMatcher(")")) });
		registerRule(MatcherRule {
		    "NamedGroupReferenceMatcher",
		    CombinationMatcher(
		        SpaceMatcher(
		            TextMatcher("\\"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        SpaceMatcher(
		            ReferenceMatcher("Identifier"),
		            false,
		            ESpaceDirection::Both,
		            ESpaceMethod::Whitespace),
		        NegativeMatcher(
		            OrMatcher(
		                CombinationMatcher(
		                    SpaceMatcher(
		                        TextMatcher("!"),
		                        false,
		                        ESpaceDirection::Both,
		                        ESpaceMethod::Whitespace),
		                    TextMatcher(";")),
		                CombinationMatcher(
		                    OptionalMatcher(
		                        SpaceMatcher(
		                            TextMatcher("?"),
		                            false,
		                            ESpaceDirection::Both,
		                            ESpaceMethod::Whitespace)),
		                    TextMatcher(":"))))) });
		registerRule(MatcherRule {
		    "ReferenceMatcher",
		    CombinationMatcher(
		        ReferenceMatcher("Identifier"),
		        NegativeMatcher(
		            OrMatcher(
		                CombinationMatcher(Tuple {
		                    SpaceMatcher(
		                        TextMatcher("!"),
		                        false,
		                        ESpaceDirection::Both,
		                        ESpaceMethod::Whitespace),
		                    TextMatcher(";") }),
		                CombinationMatcher(
		                    OptionalMatcher(
		                        SpaceMatcher(
		                            TextMatcher("?"),
		                            false,
		                            ESpaceDirection::Both,
		                            ESpaceMethod::Whitespace)),
		                    TextMatcher(":"))))) });
		registerRule(MatcherRule { "TextMatcher", RegexMatcher("\"(?:[^\"\\\\\n]|\\.|\\\\.)*\"") });
		registerRule(MatcherRule { "RegexMatcher", RegexMatcher("'(?:[^'\\\\\n]|\\.|\\\\.)*'") });
	}
} // namespace GrammarLexer