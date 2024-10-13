'use client';
import { Editor } from '@monaco-editor/react';

const EDITOR_FONT_SIZE = 17;

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
        fontFamily: 'Courier New, Lucida Console, Monaco, monospace',
      }}
      language='keywords'
      width='50%'
      value={content}
      height={'100vh'}
    />
  );
};
