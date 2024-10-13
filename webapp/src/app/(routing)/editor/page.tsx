'use client';

import { useCodeRunner } from '@/hooks/useCodeRunner';
import { Flex } from '@twilio-paste/core';
import { useEffect, useState } from 'react';
import { TextEditor } from './text-editor';
import { ResultView } from './result-view';

export default function RootPage() {
  const [code, setCode] = useState('');
  const { output, onChangeCode } = useCodeRunner();

  useEffect(() => {
    const handler = setTimeout(() => {
      if (code) {
        onChangeCode(code);
      }
    }, 250);

    return () => {
      clearTimeout(handler);
    };
  }, [code]);

  return (
    <Flex>
      <TextEditor
        onChange={(text) => (text ? setCode(text) : true)}
        keywords={['do', 'else', 'loop']}
        builtins={['std_out', 'std_map']}
      />
      <ResultView content={output} />
    </Flex>
  );
}
