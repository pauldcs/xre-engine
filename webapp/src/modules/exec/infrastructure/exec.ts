import { spawn } from 'child_process';

export const execCodeAdapter = async ({ code }: { code: string }): Promise<string> => {
  return new Promise((resolve, reject) => {
    const child = spawn('/app/shared/xre', ['-d', '-x', code]);
    let out = '';

    child.stdout.on('data', (data) => {
      out += data.toString();
    });

    child.stderr.on('data', (data) => {
      out += data.toString();
    });

    child.on('close', (code) => {
      resolve(out);
    });

    child.on('error', (error) => {
      reject(error);
    });
  });
};
