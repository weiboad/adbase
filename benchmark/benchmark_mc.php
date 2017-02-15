<?php
class BenchMc {
	// {{{ members
	
	/**
	 * concurrencys 
	 * 
	 * @var array
	 */
	protected $concurrencys = array(1, 10, 20, 50, 100);	

	// }}}
	// {{{ functions
	// {{{ public function run()
	
	/**
	 * run 
	 * 
	 * @return void
	 */
	public function run($url, $time, $threads, $method) {
		$result = array();
		foreach ($this->concurrencys as $concurrency) {
			$result[$concurrency] = $this->batchBench($url, $concurrency, $time, $threads, $method);	
		}
		return $result;
	}

	// }}}
	// {{{ protected function batchBench()
	
	/**
	 * bench 
	 * 
	 * @return void
	 */
	protected function batchBench($url, $concurrency, $time, $threads, $method) {
		$data = array(
			'empty', // empty
			'100b',
			'1k',			
			'10k',			
			'20k',			
			'30k',			
			'50k',			
		);

		$result = array();
		foreach ($data as $val) {
			$result[$val] = $this->memaslap($url, $concurrency, $time, $threads, $val, $method);
		}
		
		return $result;
	}

	// }}}
	// {{{ protected function memaslap()
	
	/**
	 * memaslap 
	 * 
	 * @return void
	 */
	protected function memaslap($url, $concurrency, $time, $threads, $data, $method) {
		if ($concurrency < $threads) {
			$threads = $concurrency;
		}
		$cmd = 'memaslap -B -s ' . $url . ' -c ' . $concurrency . ' -T ' . $threads ;
		$cmd .= ' -F ' . 'data/' . $method . $data .  ' -t ' . $time . 's';	
		echo $cmd, PHP_EOL;
		$startTime = microtime(true);
		exec($cmd, $out);
		$endTime = microtime(true);
		$exeTime = $endTime - $startTime;
		$qps = 0;
		$avg = 0;
		foreach ($out as $str) {
			if (strpos($str, 'Ops:') === false) {
				continue;	
			}
			list($line1, $line2) = explode('Ops:', trim($str));
			list($ops) = explode(' ', trim($line2));
			$tps = (int)(($ops * 1000 / $exeTime) / 1000);
			$avg = (int)(1000000 / $tps);
			break;
		}
		return array(
			'qps' => $tps,
			'avg' => $avg / 1000
		);
	}

	// }}}
	// }}}
}

$bench = new BenchMc();
$resultset = $bench->run('10.77.96.203:10012', 1, 5, 'set');
$resultget = $bench->run('10.77.96.203:10012', 1, 5, 'get');
$html = file_get_contents('tpl.html');
$dataset = json_encode($resultset);
$dataget = json_encode($resultget);
$html = str_replace('ADBASE_MC_SET_TPL', $dataset, $html);
$html = str_replace('ADBASE_MC_GET_TPL', $dataget, $html);
file_put_contents('benchmark_mc.html', $html);
