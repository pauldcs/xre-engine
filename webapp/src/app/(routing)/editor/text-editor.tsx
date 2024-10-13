'use client';
import { Editor } from '@monaco-editor/react';
import { useEffect, useState } from 'react';

const EDITOR_FONT_SIZE = 15;

const COLOR_BACKGROUND = '#ffffff';
const COLOR_FOREGROUND = '#000000';
const COLOR_CURSOR_FOREGROUND = '#000000';

const COLORS = {
  'editor.background': COLOR_BACKGROUND,
  'editor.foreground': COLOR_FOREGROUND,
  'editorCursor.foreground': COLOR_CURSOR_FOREGROUND,
};

const ops = [
  'Arithmetic Add',
  'Arithmetic Sub',
  'Arithmetic Mul',
  'Arithmetic Mod',
  'Arithmetic Div',
  'Pow',
  'Logical And',
  'Logical Or',
  'Logical Do',
  'Logical Else',
  'Logical Loop',
  'Logical Not',
  'Bitwise And',
  'Bitwise Or',
  'Bitwise Xor',
  'Bitwise Lshift',
  'Bitwise Rshift',
  'Compare LT',
  'Compare GT',
  'Compare LE',
  'Compare GE',
  'Compare EQ',
  'Compare NE',
  'Reference',
  'Sequence',
  'Closure',
  'Assign',
  'Separator',
  'Builtin Call',
  'Impossible',
];

const types = ['i64', 'vec<object_t>', 'vec<u8>', 'string', 'buffer', 'any', 'type none'];
export const TextEditor = ({
  onChange,
  keywords,
  builtins,
}: {
  onChange: (text: string | undefined) => void;
  keywords: string[];
  builtins: string[];
}) => {
  const [completionDisposable, setCompletionDisposable] = useState<any>();
  useEffect(() => {
    return () => {
      if (completionDisposable) {
        completionDisposable.dispose();
      }
    };
  }, [completionDisposable]);

  function createDependencyProposals(editor: any, range: any) {
    return [...keywords, ...builtins].flatMap((keyword) => {
      return {
        insertText: keyword,
        kind: editor.languages.CompletionItemKind.Keyword,
        label: keyword,
        range: range,
      };
    });
  }
  return (
    <Editor
      options={{
        minimap: { enabled: false },
        renderLineHighlight: 'none',
        contextmenu: false,
        fontSize: EDITOR_FONT_SIZE,
        links: true,
        autoClosingBrackets: 'always',
        autoClosingQuotes: 'always',
        foldingHighlight: false,
        padding: { top: 5 },
        roundedSelection: false,
        overviewRulerBorder: false,
        scrollBeyondLastLine: false,
        scrollPredominantAxis: false,
        scrollbar: {
          horizontalScrollbarSize: 5,
          verticalScrollbarSize: 5,
        },
      }}
      width='50%'
      height={'100vh'}
      onChange={onChange}
      language='keywords'
      beforeMount={(monaco) => {
        setCompletionDisposable(
          monaco.languages.registerCompletionItemProvider('keywords', {
            provideCompletionItems: function (model, position) {
              const word = model.getWordUntilPosition(position);
              const { lineNumber } = position;
              const { startColumn, endColumn } = word;
              return {
                suggestions: createDependencyProposals(monaco, {
                  startLineNumber: lineNumber,
                  endLineNumber: lineNumber,
                  startColumn: startColumn,
                  endColumn: endColumn,
                }),
              };
            },
          })
        );
      }}
      onMount={(editor, monaco) => {
        monaco.editor.defineTheme('editorTheme', {
          rules: [
            {
              token: 'languageKeywords',
              fontStyle: 'bold',
              foreground: '#A020F0',
            },
            {
              token: 'strings',
              fontStyle: 'bold',
              foreground: '#BC8F8F',
            },
            {
              token: 'words',
              fontStyle: 'bold',
              foreground: '#000000',
            },
            {
              token: 'languageBultins',
              fontStyle: 'bold',
              foreground: '#0000ff',
            },
            {
              token: 'outputOps',
              fontStyle: 'bold',
              foreground: '#228b22',
            },
            {
              token: 'locals',
              foreground: '#b22222',
            },
            {
              token: 'variables',
              fontStyle: 'bold',
              foreground: '#b22222',
            },
            {
              token: 'types',
              fontStyle: 'bold',
              foreground: '#5f9ea0',
            },
            {
              token: 'memory',
              fontStyle: 'bold',
              foreground: '#A020F0',
            },
            {
              token: 'comment',
              foreground: '#808080',
            },
            {
              token: 'words',
              fontStyle: 'bold',
              foreground: '#000000',
            },
          ],
          base: 'hc-light',
          inherit: true,
          colors: COLORS,
        });
        monaco.editor.setTheme('editorTheme');
        monaco.languages.register({ id: 'keywords' });
        monaco.languages.setMonarchTokensProvider('keywords', {
          tokenizer: {
            root: [
              [new RegExp('(' + keywords.join('|') + ')'), 'languageKeywords'],
              [new RegExp('(' + builtins.join('|') + ')'), 'languageBultins'],
              [new RegExp('(' + types.join('|') + ')'), 'types'],
              [new RegExp('(' + ops.join('|') + ')'), 'outputOps'],
              [new RegExp('(' + ['reserve', 'drop'].join('|') + ')'), 'memory'],
              [/"([^"]*)"/g, 'strings'],
              [/local_\d{4}/i, 'locals'],
              [/var_\d{4}/i, 'variables'],
              [/#.*?(?=\n|$)/g, 'comment'],
              [/\b(?=\w*[a-zA-Z])\w+\b/g, 'words'],
            ],
          },
        });
      }}
    />
  );
};
