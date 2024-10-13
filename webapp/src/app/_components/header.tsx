'use client';

import { Box } from '@twilio-paste/core/box';
import { Flex } from '@twilio-paste/core/flex';
import { Separator } from '@twilio-paste/core/separator';
import { Text } from '@twilio-paste/core/text';
import { ReactNode } from 'react';

export function Header({ title, children }: { title: string; children?: ReactNode }) {
  return (
    <Box>
      <Flex
        padding='space50'
        vAlignContent='center'
      >
        <Box width='100%'>
          <Text
            as='h3'
            fontSize='fontSize70'
            fontWeight='fontWeightBold'
            padding='space30'
          >
            {title}
          </Text>
        </Box>
        <Box
          display='flex'
          justifyContent='right'
          width='100%'
          paddingRight='space60'
        >
          {children}
        </Box>
      </Flex>
      <Separator orientation='horizontal' />
    </Box>
  );
}
