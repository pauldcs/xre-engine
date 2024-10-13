'use client';
import { Editor } from '@monaco-editor/react';

const EDITOR_FONT_SIZE = 15;

export const ResultView = ({ content }: { content: string }) => {
  return (
    <Editor
      options={{
        renderLineHighlight: 'none',
        contextmenu: false,
        fontSize: EDITOR_FONT_SIZE,
        links: true,
        lineNumbers: 'off',
        padding: { top: 5 },
        overviewRulerBorder: false,
        scrollBeyondLastLine: false,
        scrollPredominantAxis: false,
        scrollbar: {
          horizontalScrollbarSize: 5,
          verticalScrollbarSize: 5,
        },
      }}
      language='keywords'
      width='50%'
      value={content}
      height={'100vh'}
    />
  );
};
