import { execCode } from '@/modules/exec/applications/exec.service';
import { useEffect, useState } from 'react';

export function useCodeRunner() {
  const [code, setCode] = useState('');
  const [output, setOutput] = useState('');
  const [error, setError] = useState(false);

  const runCode = async () => {
    try {
      if (code === '') {
        return;
      }

      const out = await execCode(code);
      if (out) {
        setOutput(out);
        setError(false);
      }
    } catch (error) {
      setError(true);
    }
  };

  const onChangeCode = (newCode: string) => {
    setCode(newCode);
  };

  useEffect(() => {
    runCode();
  }, [code]);

  return { output, error, onChangeCode };
}
