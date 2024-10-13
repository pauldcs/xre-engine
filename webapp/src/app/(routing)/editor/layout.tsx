'use client';
import { Header } from '@/app/_components/header';
import { Box } from '@twilio-paste/core';
import type { ReactNode } from 'react';

export default function EditorLayout({ children }: { children: ReactNode }) {
  return (
    <>
      <Header title={'XRE Engine'} />
      <Box>{children}</Box>
    </>
  );
}
