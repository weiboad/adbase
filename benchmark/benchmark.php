<?php
class Bench {
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
	public function run($url, $count, $isKeep) {
		$result = array();
		foreach ($this->concurrencys as $concurrency) {
			$result[$concurrency] = $this->batchBench($url, $concurrency, $count, $isKeep);	
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
	protected function batchBench($url, $concurrency, $count, $isKeep) {
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
			$result[$val] = $this->ab($url, $concurrency, $count, $isKeep, $val);
		}
		
		return $result;
	}

	// }}}
	// {{{ protected function ab()
	
	/**
	 * ab 
	 * 
	 * @return void
	 */
	protected function ab($url, $concurrency, $count, $isKeep = true, $data = "") {
		$cmd = 'ab -n ' . $count . ' -c ' . $concurrency;
		if ($isKeep) {
			$cmd .= ' -k ';
		}
		if ($data != 'empty') {
			$cmd .= ' -p ' . 'data/' . $data;	
		}
		$cmd .= " '$url'";
		echo $cmd, PHP_EOL;
		exec($cmd, $out);
		$qps = 0;
		$avg = 0;
		foreach ($out as $str) {
			$splitArr = explode(':', $str);
			//string(46) "Requests per second:    1610.90 [#/sec] (mean)"
			//string(42) "Time per request:       62.077 [ms] (mean)"
			if (isset($splitArr[0]) && 'Requests per second' == trim($splitArr[0])) {
				$tmp = explode(' ', trim($splitArr[1]));
				$qps = trim($tmp[0]);
			}
			if (isset($splitArr[0]) && 'Time per request' == trim($splitArr[0])) {
				$tmp = explode(' ', trim($splitArr[1]));
				$avg = trim($tmp[0]);
				break;
			}
		}
		return array(
			'qps' => $qps,
			'avg' => $avg			
		);
	}

	// }}}
	// }}}
}

$bench = new Bench();
$result = $bench->run('10.77.96.203:10010/echo', 1000000, false);
$resultkeep = $bench->run('10.77.96.203:10010/echo', 1000000, true);
$html = file_get_contents('tpl.html');
$data = json_encode($result);
$datakeep = json_encode($resultkeep);
$html = str_replace('ADBASE_TPL_KEEP', $datakeep, $html);
$html = str_replace('ADBASE_TPL', $data, $html);
file_put_contents('benchmark_http.html', $html);
